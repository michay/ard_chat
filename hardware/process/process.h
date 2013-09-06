#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "../commands/commands.h"

typedef struct
{
	ERxState eRxState;
	SMessage sMessage;

} SState;

extern SState State;

#endif
