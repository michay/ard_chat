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
		case EProcessRX_no_SOM_received:
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
	switch(msg.type)
	{
		case PC_ARD_TEXT_SOM:
			return process_text_command(msg);
	}

	return EErrorCodes_CommandTypeotSupported;
}

EErrorCodes process_text_command(SMessage msg)
{
	// Find message code
	char* data  = msg.data;
	int msg_len = msg.length;

	// get/set holder
	char get_set[4];

	char command[MAX_COMMAND_CODE];
	int status = get_next_arg_string(&data, &msg_len, command, MAX_COMMAND_CODE - 1);
	if(status == 0)
		return EErrorCodes_InvalidOpcodeMissing;

	// switch by command
	if	(strcmp(command, "nick") == 0)
	{
		// get value
		int status = get_next_arg_string(&data, &msg_len, get_set, 3);
		if(status == 0)
			return EErrorCodes_MissingArgument;

		// check if get or set
		EGetSet gstype = check_get_set(get_set);
		switch(gstype)
		{
			case EGetSet_Set:
				status = get_next_arg_string(&data, &msg_len, State.nick, MAX_NICK_LENGTH - 1);
				if(status == 0)
					return EErrorCodes_MissingArgument;
				return EErrorCodes_Success;

			case EGetSet_Get:
				return create_nick_message();

			case EGetSet_Error:
				return EErrorCodes_InvalidArgGetSet;
		}

		return EErrorCodes_UnhandledException;
	}

	return EErrorCodes_UnknownMessage;
}

char get_next_arg_string (char** data, int* data_len, char* command, int max_length)
{
	// sanity check
	if(*data_len == 0)
		return 0;

	// get argument's end
	char* orig = *data;
	*data = strstr(*data, " ");

	// copy argument from original buffer
	int command_length = (*data == NULL) ? *data_len : (*data - orig);
	if(command_length > max_length)
		return 0;

	memcpy(command, orig, command_length);

	// null terminate
	command[command_length] = '\0';

	// advance pointer
	*data_len -= command_length;

	// move till begining of next one
	while ((*data_len > 0) && (**data == ' '))
	{
		(*data)++;
		(*data_len)--;
	}

	return 1;
}

char get_next_arg_int (char** data, int* data_len, int* result)
{
	char ires[10];
	char read_result = get_next_arg_string(data, data_len, ires, 9);

	// sanity check
	if(read_result != 1)
		return read_result;

	// convert to integer
	*result = 0;
	char* res_running = ires;
	while(*res_running)
	{
		// make sure in range
		if(*res_running < '0' || *res_running > '9')
			return 0;

		*result = (*result * 10) + (*res_running - '0');
		res_running++;
	}

	return 1;
}

//
// Helpers
//
EGetSet check_get_set(char* getset)
{
	if(strcmp("get", getset) == 0)
		return EGetSet_Get;
	else if (strcmp("set", getset) == 0)
		return EGetSet_Set;

	return EGetSet_Error;
}
