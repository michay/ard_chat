#include "rs232/rs232.h"
#include "communication/comm_thread.h"
#include "communication/rs232_interface.h"
#include "posix/posix.h"
#include "process/process.h"
#include "commands/commands.h"

#include <stdio.h>
#include <conio.h>
#include <pthread.h>

#define COMPORT 3

int main(void)
{
	// Create comm objects
	unsigned char buf[COMM_BUFFER_SIZE];
	sCommData CommData =
	{
		0, 0,

		rs232_comm_connect,
		rs232_comm_disconnect,
		rs232_comm_read_data,
		rs232_comm_send_data,

		PTHREAD_MUTEX_INITIALIZER,

		buf
	};

	// Create thread for communication
	pthread_t* comm_thread = make_thread(comm_thread_exec, (void *)&CommData);

	RS232_SendByte(COMPORT, '\r'); RS232_SendByte(COMPORT, '\n');
	while(1)
	{
		unsigned char r = getch();

		if(r == 27)
			break;

		RS232_SendByte(COMPORT, r);

		if(r == 0x0A || r == 0x0D)
		{
			ETerminalMessageStatus status = process_terminal_command();
			if (status == ETerminalMessageStatus_ValidMessage)
			{
				printf("\r\n->[%d]", tx_pc_ard_length);
				RS232_SendBuf(COMPORT, tx_pc_ard_buffer, tx_pc_ard_length);
			}
		}
		else
		{
			process_terminal_char(r);
		}

	}

	printf("\r\nquitting...");

	safe_pthread_mutex_lock(&CommData.LockObj);
	CommData.bKeepGoing = 0;
	safe_pthread_mutex_unlock(&CommData.LockObj);

	safe_pthread_join(*comm_thread, NULL);

	return 0;
}
