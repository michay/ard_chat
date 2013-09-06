#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Object creation
//
void* check_malloc(size_t size)
{
	void* result = malloc(size);
	if(result == NULL)
		ON_ERROR(-1);

	return result;
}
