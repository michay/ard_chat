#include "process.h"
#include "../utils/error_codes.h"

#include <string.h>
#include <stdlib.h>

SState State;
STerminalInterface TerminalInterface;
sBuffers Buffers;

void handle_terminal_comm(unsigned char rx)
{
	// process received character
	EProcessRX status = process_char(rx);

	// Switch action
	switch(status)
	{
		// General terminal message
		case EProcessRX_no_sync_received:
		{
			// Write char back, no special terminal support
			char one_char_str[2];
			one_char_str[0] = rx;
			one_char_str[1] = '\0';
			TerminalInterface.write_to_terminal(one_char_str);

			if(rx == '\r')
				State.prompt_mode = 1;
		}
			break;

		// During message reception, but not completely received yet
		// Ignore
		case EProcessRX_message_not_ready:
			// Ignore
			break;

			// Successfully received message
		case EProcessRX_success:
		{
			EErrorCodes process_result = process_command();
			if(process_result != EErrorCodes_Success)
				TerminalInterface.report_error("NACK - ", process_result);
			else
				TerminalInterface.write_to_terminal("\r\nACK");
		}
			break;

		// Error parsing message
		case EProcessRX_invald_checksum:
			TerminalInterface.report_error(NULL, EErrorCodes_InvalidChecksum);
			break;
	}

	// Prompt if nothing for write is pending
	if(State.prompt_mode && Buffers.ard_pc_buffer.buff_size == 0)
	{
		TerminalInterface.write_to_terminal("\nchat> ");
		State.prompt_mode = 0;
	}
}

EErrorCodes process_command(void)
{
	// Get message
	SMessage msg = State.sMessage;

	// Decide by message code
	switch(msg.opcode)
	{
		case ECommandCodes_Nick:
			//
			// check get / set
			//
			if(msg.data[0] != EGetSet_Get && msg.data[0] != EGetSet_Set)
				return EErrorCodes_InvalidArgGetSet;

			// switch by type
			switch(msg.data[0])
			{
				case EGetSet_Get:
					// send back nickname
					create_nick_command();
					return EErrorCodes_Success;

				case EGetSet_Set:
					// check length matches
					if(msg.length - 1  > MAX_NICK_LENGTH || (msg.length - 1) == 0)
						return EErrorCodes_InvalidArgString;

					// set nickname
					memcpy(State.nick, msg.data + 1, msg.length - 1);
					State.nick[msg.length] = '\0';
					return EErrorCodes_Success;
			}
	}

	return EErrorCodes_CommandTypeNotSupported;
}
