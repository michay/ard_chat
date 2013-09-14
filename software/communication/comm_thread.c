#include "comm_thread.h"
#include "comm_defenitions.h"
#include "../posix/posix.h"
#include "../process/process.h"
#include "../utils/buffers.h"
#include "../commands/commands.h"

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
		printf("\r\nFailed connecting to com port");
		return NULL;
	}

	// Set state and signal of conection
	safe_pthread_mutex_lock(&comm->lock_obj);

	// set state
	comm->bConnected = 1;
	comm->bKeepGoing = 1;
	KeepGoing = comm->bKeepGoing;

	// end critical section
	safe_pthread_mutex_unlock(&comm->lock_obj);

	// Loop endlessly
	while(KeepGoing)
	{
		char no_action = 1;

		// Check if anything to read
		int read_bytes = comm->read_data(Buffers.ard_to_pc_buffer.buff, MAX_TERMINAL_RX_BUFFER_SIZE - 1);
		if(read_bytes > 0)
		{
			for(int i = 0; i < read_bytes; ++i)
			{
				unsigned char ch = Buffers.ard_to_pc_buffer.buff[i];
				EProcessRX process_result = handle_received_byte(comm, ch);
				switch(process_result)
				{
					// received terminal char
					case EProcessRX_no_sync_received:
						printf("%c", (char)ch);
						break;

					case EProcessRX_success:
						printf("\r\n%s", comm->last_received_message.data);
						break;

					case EProcessRX_message_not_ready:
						// ignore - message not yet complete
						break;

					case EProcessRX_invald_checksum:
						printf("Invalid checksum error!");
						break;
				}
			}

			// dont sleep
			no_action = 0;
		}

		// check if anything to write
		int write_size = get_treaded_buff_size(&Buffers.pc_to_ard_buffer);
		if(write_size > 0)
		{
			// send buffer thread safe
			action_on_buffer(&Buffers.pc_to_ard_buffer, send_tx_buff, comm);

			// don't sleep
			no_action = 0;
		}

		// sleep 1 mili second
		if(no_action)
		{
			Sleep(1);
		}

		// Check if keep going
		safe_pthread_mutex_lock  (&comm->lock_obj);
		KeepGoing = comm->bKeepGoing;
		safe_pthread_mutex_unlock(&comm->lock_obj);
	}

	// Disconnect from comm port
	comm->disconnect();
	return NULL;
}

void send_tx_buff(void* arg)
{
	// Get communication object
	sCommDatPtr comm = (sCommDatPtr)arg;

	// send message
	comm->send_data(Buffers.pc_to_ard_buffer.buff, Buffers.pc_to_ard_buffer.buff_size);

	// reset out buffer
	Buffers.pc_to_ard_buffer.buff_size = 0;
}
