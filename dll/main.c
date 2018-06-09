#include "DLL_unsafe.h"
#include "thread_test.h"


void main(void)
{
	dll_t * list = dll_init();
	int n_threads = 8;
	int n_elements = 10;
	int64_t n_removed = 0;
	void * retval;
	pthread_t * threads = malloc(sizeof(pthread_t)*n_threads);
	
	

	if (!list)
	{
		printf("No list\n");
	}
	for (int i = 0; i < n_elements; i++)
	{
		enqueue(list, (void*) i);
	}

	for (int i = 30; i < 50; i++)
	{
		push(list, (void*) ( i  % n_elements));
	}


	printf("List size: %lu\n",list->size);

	node_t * current = first(list);

	while (current && current != list->tail)
	{
		printf("Element: %i\n", (int) current->val);
		current = current->prev;
	}
	
	int64_dll_args_t args = 
	{
		.list = list,
		.element = 7
	};

	for (int i; i < n_threads; i++)
	{
		pthread_create(&threads[i], NULL, remove_number, &args);
	}

	for (int i; i < n_threads; i++)
	{
		pthread_join(threads[i], &retval);
		printf("Thread %i, n removed %li", (int) retval);
	}
}
