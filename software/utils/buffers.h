#pragma once

#include <pthread.h>

#define MAX_TERMINAL_RX_BUFFER_SIZE 255
#define PC_TO_ARD_BUFFER_MAX_SIZE   255
#define ARD_TO_PC_BUFFER_MAX_SIZE   4096

typedef struct
{
	unsigned char* buff;
	int buff_size;

} sSimpleBuffer;

typedef struct
{
	unsigned char* buff;
	int buff_size;
	pthread_mutex_t lock_obj;

} sTreadedBuffer;

typedef struct
{
	sSimpleBuffer  terminal_buffer;
	sSimpleBuffer  ard_to_pc_buffer;
	sTreadedBuffer pc_to_ard_buffer;
} sBuffers;


void add_char_to_threaded_buff(sTreadedBuffer* buff, unsigned char ch);
int get_treaded_buff_size(sTreadedBuffer* buff);
void action_on_buffer(sTreadedBuffer* buff, void (*action)(void* use_arg), void* arg);
