#pragma once

#define MAX_TERMINAL_RX_BUFFER_SIZE 255

typedef enum
{
	ETerminalMessageStatus_ValidMessage,
	ETerminalMessageStatus_EmptyMessage,
	ETerminalMessageStatus_UnknownMessage
} ETerminalMessageStatus;

void process_terminal_char(unsigned char ch);
ETerminalMessageStatus process_terminal_command(void);
