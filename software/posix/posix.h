#pragma once

#include <pthread.h>

pthread_t* make_thread(void *(*start_routine) (void *), void *arg);

int safe_pthread_mutex_lock    (pthread_mutex_t* mutex);
int safe_pthread_mutex_unlock  (pthread_mutex_t* mutex);

int safe_pthread_join   (pthread_t thread, void** value_ptr);
