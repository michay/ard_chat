#include "process.h"
#include "../commands/commands.h"

#include <string.h>

static unsigned char terminal_buffer[MAX_TERMINAL_RX_BUFFER_SIZE];
static unsigned char buffer_index;

void process_terminal_char(unsigned char ch)
{
	// check if command
	if(buffer_index == 0 && (char)ch !=  '/')
		return;

	terminal_buffer[buffer_index++] = ch;
}

ETerminalMessageStatus process_terminal_command()
{
	// make sure any data exists
	if(buffer_index == 0)
		return ETerminalMessageStatus_EmptyMessage;

	// make sure null terminated
	unsigned char message_length = buffer_index;
	terminal_buffer[buffer_index] = '\0';

	// reset buffer index
	buffer_index = 0;

	// create message
	create_message_to_ard(terminal_buffer, message_length);
	return ETerminalMessageStatus_ValidMessage;
}
