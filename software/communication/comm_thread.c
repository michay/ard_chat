#include "comm_thread.h"
#include "../posix/posix.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void* comm_thread_exec(void* arg)
{
	// Local variables
	int status;
	unsigned char KeepGoing;

	// Get communication object
	sCommDatPtr comm = (sCommDatPtr)arg;

	// Try connecting
	status = comm->connect();
	if(status != 0)
	{
		printf("\rFailed connecting to com port");
		return NULL;
	}

	// Set state
	comm->bConnected = 1;

	safe_pthread_mutex_lock(&comm->LockObj);
	comm->bKeepGoing = 1;
	KeepGoing = comm->bKeepGoing;
	safe_pthread_mutex_unlock(&comm->LockObj);

	// Loop endlessly
	while(KeepGoing)
	{
		// Check if anything to read
		memset(comm->buffer, 0, sizeof(comm->buffer[0]) * COMM_BUFFER_SIZE);
		int read_bytes = comm->read_data(comm->buffer, COMM_BUFFER_SIZE);
		if(read_bytes > 0)
		{
			// Print anything there is to print
			printf("%s", comm->buffer);
		}
		else
		{
			// sleep 1 mili second
			Sleep(1);
		}

		// Check if keep going
		safe_pthread_mutex_lock(&comm->LockObj);
		KeepGoing = comm->bKeepGoing;
		safe_pthread_mutex_unlock(&comm->LockObj);
	}

	// Disconnect from comm port
	comm->disconnect();
	return NULL;
}
