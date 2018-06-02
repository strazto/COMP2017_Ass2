#include "test.h"
#include <stdlib.h>
#include <stdio.h>

static post * example_1_posts()
{
	size_t n_posts = 20;
	uint64_t i = 0;
	post* posts = calloc(n_posts, sizeof(post));
	post* p = NULL;

	for (i = 0; i < n_posts; i++)
	{
		posts[i].pst_id = i;
		LOG_V("Setting id of:%lu to post idx: %ld\n", posts[i].pst_id, i);
	}

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
	return posts;
}


void main (void)
{
	query_helper * q_h = malloc(sizeof(query_helper));
	result * res = NULL;
	post ** post_res = NULL;
	
	
	
	res = find_all_reposts(posts, n_posts, 2, q_h);
	
	post_res = (post**) res->elements;
	
	fflush(stdout);
	for (i = 0; i < res->n_elements ; i++)
	{
		printf("%lu ",post_res[i]->pst_id);
	}
	
}
