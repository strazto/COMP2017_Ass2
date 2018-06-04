#include "DLL_unsafe.h"
#include "thread_test.h"
#include "log.h"
#include <cmocka.h>

void main(void)
{
	dll_t * list = dll_init();
	int n_threads = 8;
	int n_elements = 10;
	int64_t n_removed = 0;
	void * retval;
	pthread_t * threads = malloc(sizeof(pthread_t)*n_threads);
	int to_push = 0;
	int64_t count = 0;	

	if (!list)
	{
		printf("No list\n");
	}
	for (int i = 0; i < n_elements; i++)
	{
		enqueue(list, (void*) i);
	}

	for (int i = 0; i < 50; i++)
	{
		to_push = i;
		if (! (i %3)) to_push = 7;

		push(list, (void*) ( to_push));
	}


	printf("List size: %lu\n",list->size);

	node_t * current = first(list);

	while (current && current != list->tail)
	{
		printf("Element: %li\n", (int64_t) current->val);
		if (7 == (int64_t) current->val) count++;
		current = current->prev;
	}
	printf("N 7s %li", count);
	count = 0;
	int64_dll_args_t args = 
	{
		.list = list,
		.element = 7
	};

	for (int i = 0; i < n_threads; i++)
	{
		LOG_I("Initializing thread %i", i);
		pthread_create(&threads[i], NULL, remove_number, &args);
	}

	for (int i = 0; i < n_threads; i++)
	{
		pthread_join(threads[i], &retval);
		LOG_V("Thread %i, n removed %li", i,(int64_t) retval);
		count += (int64_t) retval;

	}
	printf("N removed total: %li", count);

	printf("List size: %lu\n",list->size);
	for (current = first(list); current && current != list->tail; current = current->prev)
	{
		printf("Element: %li\n", (int64_t) current->val);		
	}
}


