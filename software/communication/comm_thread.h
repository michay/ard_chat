#pragma once

#include <pthread.h>

#define COMM_BUFFER_SIZE 4096

typedef struct sCommData* sCommDatPtr;

typedef struct sCommData
{
	//
	// State
	//
	unsigned char bConnected;
	unsigned char bKeepGoing;

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
	pthread_mutex_t LockObj;

	//
	// Buffer
	//
	unsigned char* buffer;

} sCommData;

void* comm_thread_exec(void* arg);
