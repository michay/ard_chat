#pragma once

#define ARD_PC_BUFFER_MAX_SIZE 255

typedef struct
{
	unsigned char* buff;
	int buff_size;

} sSimpleBuffer;

typedef struct
{
	sSimpleBuffer ard_pc_buffer;
} sBuffers;
