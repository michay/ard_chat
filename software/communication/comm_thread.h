#ifndef __COMM_THREAD_H__
#define __COMM_THREAD_H__

#include "../commands/commands.h"
#include "comm_defenitions.h"

#include <pthread.h>

void* comm_thread_exec(void* arg);

void send_tx_buff(void* arg);

#endif
