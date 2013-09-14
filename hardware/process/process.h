#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "../commands/commands.h"
#include "../utils/error_codes.h"
#include "../utils/buffers.h"

#define MAX_NICK_LENGTH 10

typedef struct
{
	ERxState eRxState;
	SMessage sMessage;
	char nick[MAX_NICK_LENGTH+1];
	char prompt_mode;
} SState;

typedef struct
{
	void (*write_to_terminal)(char* str);
	void (*report_error)(char* str, EErrorCodes error_code);
} STerminalInterface;

extern SState State;
extern STerminalInterface TerminalInterface;
extern sBuffers Buffers;

#ifdef __cplusplus
extern "C"
{
#endif


//
// Command processing
//
EErrorCodes process_command(void);
EErrorCodes process_text_command(SMessage msg);

//
// Terminal communication
//
void handle_terminal_comm(unsigned char rx);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
