#include "posix.h"
#include "../utils.h"
#include <pthread.h>
#include <stdlib.h>

pthread_t* make_thread(void *(*start_routine) (void *), void *arg)
{
	int n;
	pthread_t* thread = (pthread_t *)check_malloc(sizeof(pthread_t));
	n = pthread_create (thread, NULL, start_routine, arg);
	if (n != 0)
	{
		ON_ERROR (-1);
	}

	return thread;
}

int safe_pthread_mutex_lock (pthread_mutex_t* mutex)
{
	int status = pthread_mutex_lock(mutex);
	if(status != 0)
		ON_ERROR(status);

	return status;
}

int safe_pthread_mutex_unlock (pthread_mutex_t* mutex)
{
	int status = pthread_mutex_unlock(mutex);
	if(status != 0)
		ON_ERROR(status);

	return status;
}

int safe_pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex)
{
	int status = pthread_cond_wait(cond, mutex);
	if(status != 0)
		ON_ERROR(status);

	return status;
}

int safe_pthread_cond_signal(pthread_cond_t *cond)
{
	int status = pthread_cond_signal(cond);
	if(status != 0)
		ON_ERROR(status);

	return status;
}

int safe_pthread_join   (pthread_t thread, void** value_ptr)
{
	int status = pthread_join(thread, value_ptr);
	if(status != 0)
		ON_ERROR(status);
	return status;
}

