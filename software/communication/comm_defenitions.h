#pragma once

#include <pthread.h>

#define MAX_MESSAGE_SIZE 30
#define SYNC_CODE 0x55

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


typedef struct
{
	ECommandCodes opcode;
	char data[MAX_MESSAGE_SIZE+1];
	int length;
} SMessage;

typedef struct sCommData* sCommDatPtr;
typedef struct sCommData
{
	//
	// State
	//
	unsigned char bConnected;
	unsigned char bKeepGoing;

	//
	// Message
	//
	ERxState rx_state;
	SMessage last_received_message;

	//
	// Connection methods
	//
	int  (*connect)    (void);
	void (*disconnect) (void);
	int  (*read_data) (unsigned char*, int);
	int  (*send_data) (unsigned char*, int);

	//
	// Thread safety
	//
	pthread_mutex_t lock_obj;
	pthread_cond_t  connected_cond;

} sCommData;
