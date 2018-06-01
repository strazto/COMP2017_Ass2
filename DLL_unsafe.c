#include "DLL_unsafe.h"

dll_t * dll_init()
{
	dll_t * out = malloc(sizeof(dll_t));
	out->head = calloc(1, sizeof(node_t));
	out->tail = calloc(1, sizeof(node_t));
	
	out->tail->next = out->head;
	out->head->prev = out->tail;

	out->size = 0;
	return out;
}

void dll_destroy(dll_t * list)
{
	free(list->head);
	free(list->tail);
	free(list);
}

node_t * dll_insert(dll_t * list, node_t * prev, node_t * next, void* val)
{
	if (!list || !prev || !next) return NULL;

	node_t * new = malloc(sizeof(node_t));
	new->val = val;
	new->prev = prev;
	new->next = next;
	
	list->size++;

	return new;
}

node_t * dll_insert_after(dll_t * list, node_t * prev, void * val)
{
	node_t * next = prev->next;
	node_t * out  = dll_insert(list, prev, next, val);
	return out;
}

node_t * dll_insert_before(dll_t * list, node_t * next, void * val)
{
	node_t * prev = next->prev;
	node_t * out = dll_insert(list, prev, next, val);
	return out;
}

in enqueue(dll_t * list, void * value)
{
	if (!list) return NULL;
	
	
}



