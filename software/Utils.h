#pragma once

#include <stdio.h>
#include <stdlib.h>

#define ON_ERROR(exitcode) exit(exitcode)

void* check_malloc(size_t size);
