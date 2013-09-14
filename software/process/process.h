#pragma once

#include "../utils/buffers.h"
#include "../utils/error_codes.h"

#define MAX_TERMIANL_COMMAND_SIZE 20

#define MAX_NICK_LENGTH 10

typedef enum
{
	EGetSet_Get,
	EGetSet_Set,
	EGetSet_Error
}EGetSet;

// process key
void process_terminal_char(unsigned char ch);

// process terminal line
EErrorCodes process_terminal_line(unsigned char* line, int len);


//
// Text parsing
//
char get_next_arg_string (char** data, int* data_len, char* command, int max_length);
char get_next_arg_int (char** data, int* data_len, int* result);
EGetSet check_get_set(char* getset);

extern sBuffers Buffers;
