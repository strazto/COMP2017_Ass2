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
	return(find_all_reposts_wrapper(posts, count, post_id, helper));
}

result* find_original(post* posts, size_t count, uint64_t post_id, query_helper* helper) {
	
	return find_original_wrapper(posts, count, post_id, helper);
}

result* shortest_user_link(user* users, size_t count, uint64_t userA, uint64_t userB, query_helper* helper) {
	return NULL;
}

result* find_bots(user* users, size_t user_count, post* posts, size_t post_count, criteria* crit, query_helper* helper) {
	return NULL;
}

void engine_cleanup(query_helper* helpers) {
	//Clean up your engine
	free(helpers->threads);
	free(helpers);
}


