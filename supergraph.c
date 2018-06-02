#include "supergraph.h"
#include "mine.h"
#include "log.h"
query_helper* engine_setup(size_t n_processors) {
	query_helper * out = (query_helper*) malloc(sizeof(query_helper)*n_processors);
	out->n_threads = n_processors;
	out->threads = (pthread_t*) malloc(sizeof(pthread_t)*n_processors);
	
	//TODO: Add semaphores and mutexes at some stage
	
	return out;
}

/*
Find all reposts of a given post, 
*/
result* find_all_reposts(post* posts, size_t count, uint64_t post_id, query_helper* helper) 
{
	args_t * args;
	//Find the original post
	bst_args_t * idx = malloc(sizeof(bst_args_t)); 
	idx->arr = (void*) posts;
	idx->lo = 0;
	idx->hi = count;// -1;
	idx->arr_type = POST_ARR;
	idx->id = post_id;
	int64_t post_idx = (int64_t) find_idx((void*) idx);
	
	LOG_I("Finished searching for post with id of %lu, found at index %li", idx->id, post_idx);
	
	//Base case
	if (post_idx < 0) return NULL;
	
	
	
	helper->posts = posts;
	helper->post_count = count;
	helper->res = malloc(sizeof(result));
	helper->res->elements = calloc(count, sizeof(post*));
	helper->res->n_elements = 0;
	
	args = malloc(sizeof(args_t));
	args->q_h = helper;
	args->current_post = &(args->q_h->posts[post_idx]);
	LOG_I("Beginning recursive search for resposts, starting from %lu", post_idx);
	//Now begin the recursion
	find_all_reposts_r((void*) args);
	return args->q_h->res;
}

result* find_original(post* posts, size_t count, uint64_t post_id, query_helper* helper) {
	return NULL;
}

result* shortest_user_link(user* users, size_t count, uint64_t userA, uint64_t userB, query_helper* helper) {
	return NULL;
}

result* find_bots(user* users, size_t user_count, post* posts, size_t post_count, criteria* crit, query_helper* helper) {
	return NULL;
}

void engine_cleanup(query_helper* helpers) {
	//Clean up your engine
}


