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
	unsigned char valid_sync_code = (rx == SYNC_CODE);

	// Check if pending a message
	if(State.eRxState == ERxState_Sync && !valid_sync_code)
		return EProcessRX_no_sync_received;

	// switch by machine state
	switch(State.eRxState)
	{
		case ERxState_Sync:
			// Start counting checksum
			incoming_checsum = rx;

			// Change state
			State.eRxState = ERxState_Opcode;
			break;

		case ERxState_Opcode:
			// set message code
			State.sMessage.opcode = rx;
			State.sMessage.length = 0;

			// append to checksum
			incoming_checsum += rx;

			// change state
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
				State.eRxState = ERxState_Sync;
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
			State.eRxState = ERxState_Sync;

			// see if resets to 0 [two`s complement checksum]
			return (incoming_checsum == 0) ? EProcessRX_success : EProcessRX_invald_checksum;
	}

	return EProcessRX_message_not_ready;
}

EErrorCodes create_command(char opcode, char* data, int len)
{
	// Get buffer attributes
	unsigned char* buff = Buffers.ard_pc_buffer.buff;
	int index  = Buffers.ard_pc_buffer.buff_size;

	// Check overflow
	if(index + 4 + len > ARD_PC_BUFFER_MAX_SIZE)
	{
		// General exception
		return EErrorCodes_BufferOverflow;
	}

	// Start filling buffer
	buff[index++] = SYNC_CODE;
	buff[index++] = opcode;
	buff[index++] = len;
	memcpy(buff + index, data, len);
	index += len;

	unsigned char checksum = 0;
	int i;
	for(i = Buffers.ard_pc_buffer.buff_size; i < index; ++i)
		checksum += buff[i];
	buff[index++] = ~checksum + 1;

	Buffers.ard_pc_buffer.buff_size = index;

	return EErrorCodes_Success;
}

EErrorCodes create_nick_command()
{
	return create_command(ECommandCodes_Nick, State.nick, strlen(State.nick));
}
