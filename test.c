#include "test.h"
#include <stdlib.h>
#include <stdio.h>

void main (void)
{
	query_helper * q_h = malloc(sizeof(query_helper));
	result * res = NULL;
	post ** post_res = NULL;
	size_t n_posts = 20;
	uint64_t i = 0;
	int64_t out = 0;
	post* posts = calloc(n_posts, sizeof(post));
	post* p = NULL;
	for (i = 0; i < n_posts; i++)
	{
		posts[i].pst_id = i;
		printf("%lu\n", posts[i].pst_id);
	}
	
	
	bst_args_t * args = malloc(sizeof(bst_args_t));
	args->hi= n_posts;
	args->lo= 0;
	
	args->id = 4;
	args->arr = (void*) posts;
	args->arr_type = POST_ARR;
	
	out = (int64_t) find_idx((void*) args);
	printf("%li\n", out);
	
	printf("segfault? ln: %lu\n", __LINE__);
	fflush(stdout);
	p = &posts[2];
	p->n_reposted = 5;
	p->reposted_idxs = malloc(sizeof(size_t)*p->n_reposted);
	p->reposted_idxs[0] = 4;
	p->reposted_idxs[1] = 6;
	p->reposted_idxs[2] = 7;
	p->reposted_idxs[3] = 10;
	p->reposted_idxs[4] = 11;	
	p = &posts[7];
	p->n_reposted = 3;
	p->reposted_idxs = malloc(sizeof(size_t)*p->n_reposted);
	p->reposted_idxs[0] = 15;
	p->reposted_idxs[1] = 17;
	p->reposted_idxs[2] = 13;
	
	
	res = find_all_reposts(posts, n_posts, 2, q_h);
	
	post_res = (post**) res->elements;
	
	fflush(stdout);
	for (i = 0; i < res->n_elements ; i++)
	{
		printf("%lu ",post_res[i]->pst_id);
	}
	
}
