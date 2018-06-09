#ifndef THREAD_TEST_H
#define THREAD_TEST_H
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "DLL.h"
#include <unistd.h>


typedef struct int64_dll_args int64_dll_args_t;

struct int64_dll_args 
{
	dll_t * list;
	int64_t element;
};

/* 
 * ::args: argument structure (of thread_args_t) type, containing number, and list applicable 
 */
void * remove_number(void * args);


#endif
