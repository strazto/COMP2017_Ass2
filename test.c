#include "test.h"
#include <stdlib.h>
#include <stdio.h>




void main (void)
{
	const Unit_Tests[] tests = 
	{
		unit_test(ex1_test_find_all_reposts)
	};
	run_tests(tests);
}


static ex_props_t * example_1_posts()
{
	
	ex_props_t * out = malloc(sizeof(ex_props_t));

	
	size_t n_posts = 20;
	uint64_t i = 0;
	post* posts = calloc(n_posts, sizeof(post));
	post* p = NULL;
	
	out->posts = posts;
	out->n_posts = n_posts;

	io_sample_t * ex = NULL;
	
	for (i = 0; i < n_posts; i++)
	{
		posts[i].pst_id = i;
		LOG_V("Setting id of:%lu to post idx: %ld\n", posts[i].pst_id, i);
	}

	//Do find reposts
	ex = &(out->find_reposts);
	ex->n_examples = 1;
	ex->input_ids = malloc(sizeof(int64_t)*ex->n_examples);
	ex->expected_outputs  = malloc(sizeof(result)*ex->n_examples);

	ex->input_ids[0] = posts[2].pst_id;
	ex->expected_outputs[0].n_elements = 9;
	ex->expected_outputs[0].elements = malloc(ex->expected_outputs[0].n_elements*sizeof(post*));
	post ** res = (post**) ex->expected_outputs[0].elements;

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
	
	res[0] = &posts[2];
	res[1] = &posts[4];
	res[2] = &posts[6];
	res[3] = &posts[7];
	res[4] = &posts[10];
	res[5] = &posts[11];

	res[6] = &posts[15];
	res[7] = &posts[17];
	res[8] = &posts[13];

	return out;
}

static void ex1_test_find_all_reposts(void** state)
{
	(void) state;

	int64_t i = 0;
	query_helper * q_h = engine_setup(8);

	ex_props_t * props = example_1_posts();
	post * posts = props->posts;
	size_t n_posts = props->n_posts;
	result * expected = NULL;
	result * actual = NULL;

	io_sample_t * ex = &(props->find_reposts);


	for (i = 0; i < ex->n_examples; i++)
	{
		expected = &(ex->expected_outputs[i]);
		actual = find_all_reposts(posts, n_posts, ex->input_ids[i], q_h);

		assert_int_equals(expected->n_elements, actual->n_elements);

		for (int64_t j = 0; j < actual->n_elements; j++)
		{
			assert_in_set(actual->elements[j], expected->elements, expected->n_elements);
		}
	}



	teardown_posts(posts);
}

static void teardown_posts(post * posts, size_t size)
{
	size_t i = 0;
	for (i = 0; i < size ; i++)
	{
		if (posts[i].reposted_idxs)
			{
				free(posts[i].reposted_idxs);
				posts[i].reposted_idxs = NULL;
			} 
	}
	free(posts);
}