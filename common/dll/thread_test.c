#include "thread_test.h"
#include "log.h"


void * remove_number(void * args)
{
	usleep(pthread_self() %10);
	
	int64_dll_args_t * int_args = (int64_dll_args_t*) args;
	int64_t count = 0;
	dll_t * list = int_args->list;
	int64_t val = int_args->element;

	node_t * current = last(list);
	node_t * prev = current;

	while (current && current != list->head)
	{
		
		LOG_D("Traversing list, current val is %ld", (int64_t) current->val);
		prev = current;
		current = current->next;
		if (val == (int64_t) prev->val)
		{
			
			dll_remove(list, prev);
			count++;
		}

	}

	return (void*) count;
}
