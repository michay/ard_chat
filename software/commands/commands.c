#include "commands.h"
#include "../process/process.h"
#include "../posix/posix.h"
#include "../communication/comm_thread.h"
#include "../communication/comm_defenitions.h"

#include <string.h>

static unsigned char incoming_message_length;
static unsigned char incoming_checsum;

EProcessRX handle_received_byte (sCommData* comm, unsigned char rx)
{
	char valid_sync_code = (rx == SYNC_CODE);

	// Check if pending a message
	if(comm->rx_state == ERxState_Sync && !valid_sync_code)
		return EProcessRX_no_sync_received;

	SMessage* msg = &comm->last_received_message;

	// switch by machine state
	switch(comm->rx_state)
	{
		case ERxState_Sync:
			// Start counting checksum
			incoming_checsum = rx;

			// Change state
			comm->rx_state = ERxState_Opcode;
			break;

		case ERxState_Opcode:
			// Set message code
			msg->opcode = rx;
			msg->length = 0;

			// counting checksum
			incoming_checsum += rx;

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
				comm->rx_state = ERxState_Sync;
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
			comm->rx_state = ERxState_Sync;

			// see if resets to 0 [two`s complement checksum]
			return (incoming_checsum == 0) ? EProcessRX_success : EProcessRX_invald_checksum;
	}

	return EProcessRX_message_not_ready;
}

void create_command(ECommandCodes opcode, char* data, int len)
{
	sTreadedBuffer* buff = &Buffers.pc_to_ard_buffer;

	// critical section
	safe_pthread_mutex_lock  (&buff->lock_obj);

	int index = buff->buff_size;
	buff->buff[index++] = SYNC_CODE;
	buff->buff[index++] = opcode;
	buff->buff[index++] = len;
	memcpy(buff->buff + index, data, len);
	index += len;

	unsigned char running_checksum = 0;
	for(int i = buff->buff_size; i < index; ++i)
		running_checksum += buff->buff[i];

	// set checksum
	buff->buff[index++] = ~running_checksum + 1;

	// set size
	buff->buff_size = index;

	// critical section
	safe_pthread_mutex_unlock  (&buff->lock_obj);
}

void create_nick_command(EGetSet getset, char* new_nick, int len)
{
	char nick_data[MAX_NICK_LENGTH+2];
	nick_data[0] = getset;
	if(len > 0)
		memcpy(nick_data + 1, new_nick, len);

	create_command(ECommandCodes_Nick, nick_data, len + 1);
}
