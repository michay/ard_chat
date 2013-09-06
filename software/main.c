//#define TEST

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

#ifndef TEST
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

		if(r == 0x0A || r == 0x0D)
		{
			ETerminalMessageStatus status = process_terminal_command();
			if (status == ETerminalMessageStatus_ValidMessage)
			{
				RS232_SendBuf(COMPORT, tx_pc_ard_buffer, tx_pc_ard_length);
			}
		}
		else
		{
			process_terminal_char(r);
		}
		RS232_SendByte(COMPORT, r);
	}

	printf("\r\nquitting...");

	safe_pthread_mutex_lock(&CommData.LockObj);
	CommData.bKeepGoing = 0;
	safe_pthread_mutex_unlock(&CommData.LockObj);

	safe_pthread_join(*comm_thread, NULL);

	return 0;
}
#else

char get_next_arg_string (char** data, int* data_len, char* result);
char get_next_arg_int    (char** data, int* data_len, int* result);


int main(void)
{
	char test[] = "opcode 1 20 mic\0";
	int len = strlen(test);
	char opcode[10];

	char* data = test;
	get_next_arg_string(&data, &len, opcode);
	printf("\r\ncommand: `%s`", opcode);

	char bKeepGoing = 1;
	int arg_count = 0;
	while(bKeepGoing)
	{
		int arg;
		bKeepGoing = get_next_arg_int(&data, &len, &arg);
		if(bKeepGoing)
			printf("\r\n\t arg#%d: %d", arg_count, arg);
		arg_count++;
	}
}

char get_next_arg_string (char** data, int* data_len, char* command)
{
	// sanity check
	if(*data_len == 0)
		return 0;

	// get argument's end
	char* orig = *data;
	*data = strstr(*data, " ");

	// copy argument from original buffer
	int command_length = (*data == NULL) ? *data_len : (*data - orig);

	memcpy(command, orig, command_length);

	// null terminate
	command[command_length] = '\0';

	// advance pointer
	*data_len -= command_length;

	// move till begining of next one
	while ((*data_len > 0) && (**data == ' '))
	{
		(*data)++;
		(*data_len)--;
	}

	return 1;
}

char get_next_arg_int (char** data, int* data_len, int* result)
{
	char ires[10];
	char read_result = get_next_arg_string(data, data_len, ires);

	// sanity check
	if(read_result != 1)
		return read_result;

	// convert to integer
	*result = 0;
	char* res_running = ires;
	while(*res_running)
	{
		// make sure in range
		if(*res_running < '0' || *res_running > '9')
			return 0;

		*result = (*result * 10) + (*res_running - '0');
		res_running++;
	}

	return 1;
}
#endif
