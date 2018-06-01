#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * @author mstr3336
 * Implementation of a doubly linked list not intended to be thread-safe
 * To be fixed
 *
 */

typedef struct node node_t;
typedef struct double_linked_list dll_t;

/**
 * 
 * Node that stores a pointer to some value
 */

struct node
{
	node_t * prev;
	node_t * next;
	void * val;
};

/*
 *Double linked list implementation, stores head, tail and size
 */
struct double_linked_list 
{
	node_t * head;
	node_t * tail;
	uint64_t size;
};

/*
 * Standard DLL operations include 
 * 
 * INSERT OPS
 * 	enqueue(list, void * val) - add_last
 * 	push(list, void* val) - add_first
 * REMOVE OPS
 * 	pop(list) - remove_first
 * 	remove_tail(list)
 *
 * POSITIONAL
 * 	next(list, node)
 * 	prev(list, node)
 * 
 * first(list)
 * last(list)
 *
 * hasNext? hasPrev?
 *
 * is_head()?
 * is_tail()?
 *
 * private insert(node_t * prev, node_t * next, void * val);
 * get_size;
 * 
 * INITIALIZE STUFF
 * init_dll()
 * aforementioned insert
 *
 */

dll_t * dll_init();

node_t * dll_insert(node_t * prev, node_t * next);


