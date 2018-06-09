#include "DLL.h"

/**
 * @author mstr3336 -  Matthew Strasiotto
 * https://github.com/mstr3336/COMP2017_DLL_queue.git
 * Implementation of a doubly linked list not intended to be thread-safe
 * To be fixed
 * Licensed however the open license i set says. 
 * Don't plagarise this because you're allowed to use it if you reference it
 */

//Private memebers


static node_t * dll_insert(dll_t * list, node_t * prev, node_t * next, void *val);

static node_t * dll_insert_next(dll_t * list, node_t * prev, void * val);

static node_t * dll_insert_prev(dll_t * list, node_t * next, void * val);


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
	//Free all elements
	node_t * current = NULL;
	node_t * prev = current;
	for (current = first(list) ; current && current != list->tail; current = prev)
	{
		prev = current->prev;
		free(current);
	}
	free(list->head);
	free(list->tail);
	free(list);
}

static node_t * dll_insert(dll_t * list, node_t * prev, node_t * next, void* val)
{
	if (!list || !prev || !next) return NULL;

	node_t * new = malloc(sizeof(node_t));
	new->val = val;
	new->prev = prev;
	new->next = next;
	prev->next = new;
	next->prev = new;	
	list->size++;

	return new;
}

static node_t * dll_insert_next(dll_t * list, node_t * prev, void * val)
{
	node_t * next = prev->next;
	node_t * out  = dll_insert(list, prev, next, val);
	return out;
}

static node_t * dll_insert_prev(dll_t * list, node_t * next, void * val)
{
	node_t * prev = next->prev;
	node_t * out = dll_insert(list, prev, next, val);
	return out;
}

node_t * enqueue(dll_t * list, void * val)
{
	if (!list) return NULL;
	node_t * out = dll_insert_next(list, list->tail, val);
	return out;
}

node_t * push(dll_t * list, void * val)
{
	if (!list) return NULL;
	node_t * out = dll_insert_prev(list, list->head, val);
	return out;
}

void * pop(dll_t * list)
{	
	node_t * to_pop = first(list);
	if (!to_pop) return NULL;
	return dll_remove(list, to_pop);
}

node_t * first(dll_t * list)
{
	if (!list) return NULL;
	node_t * head = list->head;
	if (!head) return NULL;
	node_t * out = head->prev;
	if (out == list->tail) return NULL;
	return out;
}

node_t * last(dll_t * list)
{
	if (!list) return NULL;
	node_t * tail = list->tail;
	if (!tail) return NULL;
	node_t * out = tail->next;
	if (out == list->head) return NULL;
	return out; 
}

void * dll_remove(dll_t * list, node_t * to_rm)
{
	if (!list || !to_rm || to_rm == list->head || to_rm == list->tail) return (void*) -1;

	to_rm->prev->next = to_rm->next;
	to_rm->next->prev = to_rm->prev;
	void * out = to_rm->val;
	free(to_rm);
	list->size--;
	return out;
}
