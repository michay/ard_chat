#include "process.h"
#include "../commands/commands.h"
#include "../utils/buffers.h"
#include "../utils/error_codes.h"

#include <stdio.h>
#include <string.h>

sBuffers Buffers;

void process_terminal_char(unsigned char ch)
{
	//
	// Processes each terminal char
	//
	// Check if command at 0xa or 0xd chars
	//
	char line_end = (ch == 0x0A) || (ch == 0x0D);

	if(!line_end)
	{
		// add char to buffer
		add_char_to_simple_buff(&Buffers.terminal_buffer, ch);
	}
	else
	{
		// parse terminal line
		EErrorCodes status = process_terminal_line(Buffers.terminal_buffer.buff, Buffers.terminal_buffer.buff_size);

		//  clear buffer
		Buffers.terminal_buffer.buff_size = 0;
	}

	// send it to comm as well
	add_char_to_threaded_buff(&Buffers.pc_to_ard_buffer, ch);
}

EErrorCodes process_terminal_line(unsigned char* line, int len)
{
	// make sure any data
	if(len == 0)
		return EErrorCodes_NoCommand;

	// trim line
	while(*line == ' ' && len--)
		line++;

	// check if command
	if(*line == '/')
	{
		// check command
		char command[MAX_TERMIANL_COMMAND_SIZE+1];
		char status = get_next_arg_string((char**)&line, &len, command, MAX_TERMIANL_COMMAND_SIZE);
		if(!status)
			return EErrorCodes_NoCommand;

		if(strcmp(command, "/nick") == 0)
		{
			// check if get or set
			char cgetset[4];
			status = get_next_arg_string((char**)&line, &len, cgetset, 3);
			if(!status)
				return EErrorCodes_MissingArgument;

			EGetSet getset = check_get_set(cgetset);
			switch(getset)
			{
				case EGetSet_Get:
					// create command
					create_nick_command(getset, NULL, 0);
					return EErrorCodes_Success;
					break;

				case EGetSet_Set:
				{
					// check if nickname exists
					char new_nick[MAX_NICK_LENGTH+1];
					status = get_next_arg_string((char**)&line, &len, new_nick, MAX_NICK_LENGTH);
					if(status == 0)
						return EErrorCodes_MissingArgument;
					create_nick_command(getset, new_nick, strlen(new_nick));
					return EErrorCodes_Success;
				}
					break;

				case EGetSet_Error:
					return EErrorCodes_InvalidArgGetSet;
			}
		}

		return EErrorCodes_UnknownMessage;
	}
	// simple message
	else
	{
		return EErrorCodes_UnknownMessage;
	}

	return EErrorCodes_UnknownMessage;
}


//
// Text parsing
//
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
