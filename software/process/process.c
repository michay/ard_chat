#include "process.h"
#include "../commands/commands.h"
#include "../utils/buffers.h"

#include <stdio.h>
#include <string.h>

sBuffers Buffers;

void process_terminal_char(unsigned char ch)
{
	// check if command
	//if(Buffers.terminal_buffer.buff_size == 0 && (char)ch !=  '/' && (char)ch != '\r' && (char)ch != '\n')
		//return;
	char command_mode = (Buffers.terminal_buffer.buff_size > 0) || ((char)ch ==  '/');
	char end_command_mode = (ch == 0x0A) || (ch == 0x0D);

	// check if not end of command
	if (command_mode && !end_command_mode)
	{
		// add char to buffer
		int index = Buffers.terminal_buffer.buff_size;
		unsigned char* buff = Buffers.terminal_buffer.buff;

		buff[index++] = ch;

		Buffers.terminal_buffer.buff_size = index;
	}
	else if (end_command_mode && (Buffers.terminal_buffer.buff_size > 0))
	{
		// get buffer
		int index = Buffers.terminal_buffer.buff_size;
		unsigned char* buff = Buffers.terminal_buffer.buff;

		// make sure null terminated
		buff[index] = '\0';

		// create message from terminal buffer
		create_message_to_ard(buff, index);

		// clear terminal buffer
		Buffers.terminal_buffer.buff_size = 0;
	}

	// send input to arduino
	add_char_to_threaded_buff(&Buffers.pc_to_ard_buffer, ch);
}
