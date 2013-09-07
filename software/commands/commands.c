#include "commands.h"
#include "../process/process.h"
#include "../communication/comm_thread.h"
#include "../communication/comm_defenitions.h"

#include <string.h>

static unsigned char incoming_message_length;
static unsigned char incoming_checsum;

void create_message_to_ard(unsigned char* data, unsigned char length)
{
	unsigned char* buff = Buffers.pc_to_ard_buffer.buff;

	buff[0] = PC_ARD_SOM;
	buff[1] = length - 1;

	// skip the '/' sign
	memcpy(buff + 2, data + 1, length - 1);

	unsigned char running_checksum = 0;
	int index = 2 + (length - 1);
	for(int i = 0; i < index; ++i)
		running_checksum += buff[i];

	// set checksum
	buff[index++] = ~running_checksum + 1;

	// set index
	Buffers.pc_to_ard_buffer.buff_size = index;
}

EProcessRX handle_received_byte (sCommData* comm, unsigned char rx)
{
	char valid_sync_code = (rx == ARD_PC_SOM);

	// Check if pending a message
	if(comm->rx_state == ERxState_SOM && !valid_sync_code)
		return EProcessRX_no_SOM_received;

	SMessage* msg = &comm->last_received_message;

	// switch by machine state
	switch(comm->rx_state)
	{
		case ERxState_SOM:
			// Set message code
			msg->type = rx;
			msg->length = 0;

			// Start counting checksum
			incoming_checsum = rx;

			// Change state
			comm->rx_state = ERxState_Length;
			break;

		case ERxState_Length:
			// Validate length
			if(rx < MAX_MESSAGE_SIZE)
			{
				incoming_message_length = 0;
				msg->length = rx;
				incoming_checsum += rx;
				comm->rx_state = ERxState_Data;
			}
			else
			{
				// Invalid value - reset machine state
				comm->rx_state = ERxState_SOM;
			}
			break;

		case ERxState_Data:
			// Save data
			msg->data[incoming_message_length++] = rx;
			incoming_checsum += rx;

			// check if enough data
			if(incoming_message_length == msg->length)
			{
				// terminate string
				msg->data[incoming_message_length] = '\0';

				// set state
				comm->rx_state = ERxState_Checksum;
			}
			break;

		case ERxState_Checksum:
			// accumulate checksum
			incoming_checsum += rx;

			// reset state
			comm->rx_state = ERxState_SOM;

			// see if resets to 0 [two`s complement checksum]
			return (incoming_checsum == 0) ? EProcessRX_success : EProcessRX_invald_checksum;
	}

	return EProcessRX_message_not_ready;
}
