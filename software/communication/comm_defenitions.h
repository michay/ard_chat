#pragma once

#include <pthread.h>

#define MAX_MESSAGE_SIZE 30

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

typedef struct sCommData* sCommDatPtr;
typedef struct sCommData
{
	//
	// State
	//
	unsigned char bConnected;
	unsigned char bKeepGoing;

	ERxState   rx_state;

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
