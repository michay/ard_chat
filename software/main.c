//#define TEST

#include "rs232/rs232.h"
#include "communication/comm_thread.h"
#include "communication/comm_defenitions.h"
#include "communication/rs232_interface.h"
#include "posix/posix.h"
#include "process/process.h"
#include "commands/commands.h"
#include "utils/buffers.h"

#include <stdio.h>
#include <conio.h>
#include <pthread.h>

#define COMPORT 3

//
// Buffers
//
unsigned char terminal_buffer[MAX_TERMINAL_RX_BUFFER_SIZE];
unsigned char pc_to_ard_buff[PC_TO_ARD_BUFFER_MAX_SIZE];
unsigned char ard_to_pc_buff[ARD_TO_PC_BUFFER_MAX_SIZE];


#ifndef TEST
int main(void)
{
	//
	// setup buffers
	//

	// terminal buffer
	Buffers.terminal_buffer.buff      = terminal_buffer;
	Buffers.terminal_buffer.buff_size = 0;

	// communication buffers :: arduino -> pc
	Buffers.ard_to_pc_buffer.buff 	   = ard_to_pc_buff;
	Buffers.ard_to_pc_buffer.buff_size = 0;

	// communication buffers :: pc -> arduino
	Buffers.pc_to_ard_buffer.buff 	   = pc_to_ard_buff;
	Buffers.pc_to_ard_buffer.buff_size = 0;
	Buffers.pc_to_ard_buffer.lock_obj  = PTHREAD_MUTEX_INITIALIZER;

	//
	// communication thread creation
	//
	SMessage CommReceivedMessage;
	sCommData CommData =
	{
		0, 0,

		ERxState_Sync,
		CommReceivedMessage,

		rs232_comm_connect,
		rs232_comm_disconnect,
		rs232_comm_read_data,
		rs232_comm_send_data,

		PTHREAD_MUTEX_INITIALIZER,
		PTHREAD_COND_INITIALIZER
	};

	// Create thread for communication
	pthread_t* comm_thread = make_thread(comm_thread_exec, (void *)&CommData);

	// send first line break to get prompt
	add_char_to_threaded_buff(&Buffers.pc_to_ard_buffer, '\r');
	add_char_to_threaded_buff(&Buffers.pc_to_ard_buffer, '\n');

	// loop terminal
	unsigned char r;
	while( (r = getch()) != 27 )
	{
		// process terminal char
		process_terminal_char(r);
	}

	printf("\r\nquitting...");

	// stop communication thread
	safe_pthread_mutex_lock(&CommData.lock_obj);
	CommData.bKeepGoing = 0;
	safe_pthread_mutex_unlock(&CommData.lock_obj);
	safe_pthread_join(*comm_thread, NULL);

	return 0;
}
#else

int main(void)
{
	printf("micha>");
	printf("\r      \r");
	printf("hey");
}


#endif
