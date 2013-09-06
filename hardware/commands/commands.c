#include "commands.h"
#include "../process/process.h"

#include <string.h>

//
// Variables
//
unsigned char buffer_head;
unsigned char buffer_tail;

unsigned char incoming_message_length;
unsigned char incoming_checsum;


EProcessRX process_char(unsigned char rx)
{
	unsigned char valid_sync_code = (rx == PC_ARD_TEXT_SOM);

	// Check if pending a message
	if(State.eRxState == ERxState_SOM && !valid_sync_code)
		return EProcessRX_no_SOM_received;

	// switch by machine state
	switch(State.eRxState)
	{
		case ERxState_SOM:
			// Set message code
			State.sMessage.type = rx;
			State.sMessage.length = 0;

			// Start counting checksum
			incoming_checsum = rx;

			// Change state
			State.eRxState = ERxState_Length;
			break;

		case ERxState_Length:
			// Validate length
			if(rx < MAX_MESSAGE_SIZE)
			{
				incoming_message_length = 0;
				State.sMessage.length = rx;
				incoming_checsum += rx;
				State.eRxState = ERxState_Data;
			}
			else
			{
				// Invalid value - reset machine state
				State.eRxState = ERxState_SOM;
			}
			break;

		case ERxState_Data:
			// Save data
			State.sMessage.data[incoming_message_length++] = rx;
			incoming_checsum += rx;

			// check if enough data
			if(incoming_message_length == State.sMessage.length)
			{
				// terminate string
				State.sMessage.data[incoming_message_length] = '\0';

				// set state
				State.eRxState = ERxState_Checksum;
			}
			break;

		case ERxState_Checksum:
			// accumulate checksum
			incoming_checsum += rx;

			// reset state
			State.eRxState = ERxState_SOM;

			// see if resets to 0 [two`s complement checksum]
			return (incoming_checsum == 0) ? EProcessRX_success : EProcessRX_invald_checksum;
	}

	return EProcessRX_message_not_ready;
}

EErrorCodes create_message(char* data, int len)
{
	// Get buffer attributes
	char* buff = Buffers.ard_pc_buffer.buff;
	int index  = Buffers.ard_pc_buffer.buff_size;

	// Check overflow
	if(index + 3 + len > ARD_PC_BUFFER_MAX_SIZE)
	{
		// General exception
		return EErrorCodes_BufferOverflow;
	}

	// Start filling buffer
	buff[index++] = ARD_PC_TEXT_SOM;
	buff[index++] = len;
	memcpy(buff, data, len);
	index += len;

	unsigned char checksum = 0;
	int i;
	for(i = Buffers.ard_pc_buffer.buff_size; i < index; ++i)
		checksum += buff[i];
	buff[index++] = ~checksum + 1;

	Buffers.ard_pc_buffer.buff_size = index;

	return EErrorCodes_Success;
}

EErrorCodes create_nick_message()
{
	return create_message(State.nick, strlen(State.nick));
}
