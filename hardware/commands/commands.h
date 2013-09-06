#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "../utils/error_codes.h"

//
// Defines
//
#define RX_BUFFER_SIZE 255

#define MAX_MESSAGE_SIZE 30
#define MAX_COMMAND_CODE 10

#define PC_ARD_TEXT_SOM 0xF0
#define ARD_PC_TEXT_SOM 0xF1

typedef enum
{
	EProcessRX_success,
	EProcessRX_invald_checksum,
	EProcessRX_message_not_ready,
	EProcessRX_no_SOM_received
} EProcessRX;

typedef enum
{
	ERxState_SOM,
	ERxState_Length,
	ERxState_Data,
	ERxState_Checksum
} ERxState;

typedef struct
{
	unsigned char type;
	char data[MAX_MESSAGE_SIZE+1];
	int length;
} SMessage;

#ifdef __cplusplus
extern "C"
{
#endif

EProcessRX process_char(unsigned char rx);

//
// Termianl messages :: Creation
//
EErrorCodes create_message(char* data, int len);

EErrorCodes create_nick_message();

#ifdef __cplusplus
} // extern "C"
#endif

#endif
