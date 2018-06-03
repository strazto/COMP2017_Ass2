#include "thread_test.h"



void * remove_number(void * args)
{
	int64_dll_args_t * int_args = (int64_dll_args_t*) args;
	int64_t count = 0;
	dll_t * list = int_args->list;
	int64_t val = int_args->element;

	node_t * current = first(list);
	
	while (current && current != list->tail)
	{
		if (val == (int64_t) current->val)
		{
			dll_remove(list, current);
			count++;
		} 
	}

	return (void*) count;
}
