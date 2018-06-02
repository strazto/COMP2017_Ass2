#include "mine.h"
#include "supergraph.h"
#include "log.h"


/*
 * Find the location of an id from within an array
 * :: void* bst_argsp :: 
 * Returns id
 */
void* find_idx(void* bst_argsp)
{
	bst_args_t* args = (bst_args_t*) bst_argsp;
	uint64_t found = 0;
	uint8_t arr_type = args->arr_type;
	
	// Given the array is monotonically increasing, can do binary search tree
	// CRITICAL
	uint64_t want	= args->id;
	size_t hi 		= args->hi;
	size_t lo 		= args->lo;
	
	size_t mid		= lo + ((hi - lo)/2);
	
	//Depending on the array type, find the corresponding entry/id
	//Base, hi==lo
	if (lo > hi)
	{
		LOG_V("Didn't find %lu in array!", want);
		return (void*) -1;
	} 
	
	if (arr_type == POST_ARR)	found = ((post*)(args->arr))[mid].pst_id;
	else 						found = ((user*)(args->arr))[mid].user_id;
	

	LOG_V("Lo: %lu,\tMid: %lu,\tHi: %lu,\tMid_val: %lu,\tWanted: %lu",lo, mid, hi, found, want);
	
	
	//Base, find id at mid	
	if (want == found)
	{
		LOG_I("Found at index: %li!", mid);
		return (void*) mid;
	} 
	
	
	//Recursive case, search higher if you're too low, or low if too high
	if (want > found)	lo = mid + 1;
	else 				hi = mid - 1;
	
	//CRITICAL
	args->hi = hi;
	args->lo = lo;
	//END CRITICAL
	
	return find_idx((void*) args);
}

/*
Find all the reposts of a particular post,
takes argument of args type
*/
void* find_all_reposts_r(void* argsp)
{
	args_t * args = (args_t*) argsp;
	size_t i = 0;
	//Track who you could and couldn't thread, so you can chase up who you missed
	//uint8_t * unthreaded_children = calloc(args->current_post->n_reposted, sizeof(uint8_t));
	post * child = NULL;
	
	//Try to open a new thread to recurse, for each child, but if it doesn't work, dont worry
	for (i = 0; i < args->current_post->n_reposted ; i++)
	{
		//TODO: Make this work right
		//trysem to allocate a thread, maybe
		//For now, just recurse
		child = &args->q_h->posts[args->current_post->reposted_idxs[i]];
		find_all_reposts_r(heap_copy(args, NULL, child));
	}
	//CRITICAL
	//Add self to results
	args->q_h->res->elements[args->q_h->res->n_elements++] = (void*) args->current_post;
	//END CRITICAL
	//TODO: FIx segv here
	//free(unthreaded_children);
	LOG_D("Freeing arguments pointer for id: %lu", args->current_post->pst_id);
	free(argsp);
	return NULL;
}


/*
Copy by value to a new heap alloced struct
*/
args_t * heap_copy(args_t * from, user* new_user, post * new_post)
{
	args_t * out = malloc(sizeof(args_t));
	out->q_type = from->q_type;
	out->q_h = from->q_h;
	out->current_post = new_post;
	out->current_user = new_user;
	return out;
}
