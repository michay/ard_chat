#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "../utils/error_codes.h"

//
// Defines
//
#define RX_BUFFER_SIZE 255

#define MAX_MESSAGE_SIZE 30
#define MAX_COMMAND_CODE 10

#define SYNC_CODE 0x55

#define PC_ARD_TEXT_SOM 0xF0
#define ARD_PC_TEXT_SOM 0xF1
#define ARD_PC_MSG_SOM 0xF2

typedef enum
{
	EProcessRX_success,
	EProcessRX_invald_checksum,
	EProcessRX_message_not_ready,
	EProcessRX_no_sync_received
} EProcessRX;

typedef enum
{
	ERxState_Sync,
	ERxState_Opcode,
	ERxState_Length,
	ERxState_Data,
	ERxState_Checksum
} ERxState;

typedef enum
{
	ECommandCodes_Nick = 0
} ECommandCodes;

typedef enum
{
	EGetSet_Get,
	EGetSet_Set,
	EGetSet_Error
}EGetSet;

typedef struct
{
	ECommandCodes opcode;
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
EErrorCodes create_command(char opcode, char* data, int len);

EErrorCodes create_nick_command(void);

EErrorCodes create_message(char* sender, char* data, char* len);


#ifdef __cplusplus
} // extern "C"
#endif

#endif
