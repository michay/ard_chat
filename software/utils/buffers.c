#include "buffers.h"
#include "../posix/posix.h"

#include <pthread.h>

void add_char_to_threaded_buff(sTreadedBuffer* buffer, unsigned char ch)
{
	// critical section
	safe_pthread_mutex_lock  (&buffer->lock_obj);

	// get buffer
	unsigned char* buff = buffer->buff;
	int index = buffer->buff_size;

	// update buffer
	buff[index++] = ch;
	buffer->buff_size = index;

	// exit critical section
	safe_pthread_mutex_unlock(&buffer->lock_obj);
}

int get_treaded_buff_size(sTreadedBuffer* buffer)
{
	// prepare result
	int result = 0;

	// critical section
	safe_pthread_mutex_lock  (&buffer->lock_obj);

	result = buffer->buff_size;

	// exit critical section
	safe_pthread_mutex_unlock(&buffer->lock_obj);

	// result
	return result;
}

void action_on_buffer(sTreadedBuffer* buff, void (*action)(void* use_arg), void* arg)
{
	// critical section
	safe_pthread_mutex_lock  (&buff->lock_obj);

	action(arg);

	// exit critical section
	safe_pthread_mutex_unlock(&buff->lock_obj);}

