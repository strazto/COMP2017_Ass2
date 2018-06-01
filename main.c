#include "DLL_unsafe.h"


void main(void)
{
	dll_t * list = dll_init();

	if (!list)
	{
		printf("No list\n");
	}
	int n_elements = 10;
	for (int i = 0; i < n_elements; i++)
	{
		enqueue(list, (void*) i);
	}

	for (int i = 40; i < 50; i++)
	{
		push(list, (void*) i);
	}


	printf("List size: %lu\n",list->size);

	node_t * current = first(list);

	while (current && current != list->tail)
	{
		printf("Element: %i\n", (int) current->val);
		current = current->prev;
	}

}
