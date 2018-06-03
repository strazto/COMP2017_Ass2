#ifndef SUPERGRAPH_H
#define SUPERGRAPH_H
#include <stdlib.h>
#include <stdint.h>

#include <pthread.h>


typedef struct query_helper query_helper;
typedef struct result result;
typedef struct criteria criteria;
typedef struct user user;
typedef struct post post;
typedef struct args args_t;


/**
Query helper should be shared across queries, so contains shared data that may be reassigned
*/
struct query_helper {
	size_t n_threads;
	pthread_t * threads;
	
	//TODO: Add some sort of thread stack that refers to the next available thread
	
	post* posts;
	size_t post_count;
	user* users;
	size_t user_count;
	criteria* crit;
	result * res;
};





struct result {
	void** elements;
	size_t n_elements;
};

struct criteria {
	float oc_threshold;
	float acc_rep_threshold;
	float bot_net_threshold;
};

struct user {
	uint64_t user_id;
	size_t* follower_idxs;
	size_t n_followers;
	size_t* following_idxs;
	size_t n_following;
	size_t* post_idxs;
	size_t n_posts;
};

struct post {
	uint64_t pst_id;
	uint64_t timestamp;
	size_t* reposted_idxs;
	size_t n_reposted;
};





query_helper* engine_setup(size_t n_processors);

result* find_all_reposts(post* posts, size_t count, uint64_t post_id, query_helper* helper);

result* find_original(post* posts, size_t count, uint64_t post_id, query_helper* helper);

result* shortest_user_link(user* users, size_t count, uint64_t userA, uint64_t userB, query_helper* helper);

result* find_bots(user* users, size_t user_count, post* posts, size_t post_count, criteria* crit, query_helper* helper);

void engine_cleanup(query_helper* helpers);

/*
Worker function, for running 
*/
void* queries(void * args);
#endif
