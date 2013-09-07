#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "../communication/comm_thread.h"
#include "../communication/comm_defenitions.h"

#define PC_ARD_SOM 0xF0
#define ARD_PC_SOM 0xF1

typedef enum
{
	EProcessRX_success,
	EProcessRX_invald_checksum,
	EProcessRX_message_not_ready,
	EProcessRX_no_SOM_received
} EProcessRX;



void create_message_to_ard(unsigned char* data, unsigned char length);

EProcessRX handle_received_byte (sCommData* comm, unsigned char rx);

#endif
