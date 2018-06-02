#include "test.h"
#include <stdlib.h>
#include <stdio.h>




int main (void)
{
	const struct CMUnitTest tests[]= 
	{
		cmocka_unit_test(ex1_test_find_all_reposts)
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}


static ex_props_t * example_1_posts()
{
	
	ex_props_t * out = malloc(sizeof(ex_props_t));

	uint64_t max_children = 0;
	size_t n_posts = 20;
	uint64_t i = 0;
	post* posts = calloc(n_posts, sizeof(post));
	post* p = NULL;
	
	out->posts = posts;
	out->n_posts = n_posts;
	out->users=NULL;
	out->n_users = 0;
	io_sample_t * ex = NULL;
	
	for (i = 0; i < n_posts; i++)
	{
		posts[i].pst_id = i;
		LOG_V("Setting id of:%lu to post idx: %ld", posts[i].pst_id, i);
	}


	//Do find reposts
	ex = &(out->find_reposts);
	ex->n_examples = 4;
	ex->input_ids = malloc(sizeof(int64_t)*ex->n_examples);
	ex->expected_outputs  = malloc(sizeof(result)*ex->n_examples);

	ex->input_ids[0] = posts[2].pst_id;
	ex->expected_outputs[0].n_elements = 9;
	ex->expected_outputs[0].elements = malloc(ex->expected_outputs[0].n_elements*sizeof(post*));
	post ** res = (post**) ex->expected_outputs[0].elements;

	i = 0;


	p = &posts[2];
	res[i++] = p;
	p->n_reposted = 0;
	max_children = 5;
	p->reposted_idxs = malloc(sizeof(size_t)*max_children);
	
	res[i++] = make_repost(posts, n_posts, p, 4, max_children);
	res[i++] = make_repost(posts, n_posts, p, 6, max_children);
	res[i++] = make_repost(posts, n_posts, p, 7, max_children);
	res[i++] = make_repost(posts, n_posts, p, 10, max_children);
	res[i++] = make_repost(posts, n_posts, p, 11, max_children);
		
	p = &posts[7];
	max_children = 3;

	p->reposted_idxs = malloc(sizeof(size_t)*max_children);

	res[i++] = make_repost(posts, n_posts, p, 15, max_children);
	res[i++] = make_repost(posts, n_posts, p, 17, max_children);
	res[i++] = make_repost(posts, n_posts, p, 13, max_children);

	ex->expected_outputs[0].n_elements = i;

	ex->input_ids[1] = post[7].pst_id;
	max_children = 3;
	ex->expected_outputs[1].elements = calloc(ex->expected_outputs[1].n_elements, sizeof(post*));
	ex->expected_outputs[1].n_elements = 4;

	res = ex->expected_outputs[1].elements;
	res[0] = &post[7];
	res[1] = &post[15];
	res[2] = &post[17];
	res[3] = &post[13];

	
	return out;
}

static void ex1_test_find_all_reposts(void** state)
{
	

	int64_t i = (int64_t) state;
	query_helper * q_h = engine_setup(8);

	ex_props_t * props = example_1_posts();
	post * posts = props->posts;
	size_t n_posts = props->n_posts;
	result * expected = NULL;
	result * actual = NULL;

	io_sample_t * ex = &(props->find_reposts);


	if (i < ex->n_examples)
	{
		expected = &(ex->expected_outputs[i]);
		actual = find_all_reposts(posts, n_posts, ex->input_ids[i], q_h);

		LOG_D("Starting Test %ld", i);

		assert_int_equal(expected->n_elements, actual->n_elements);

		for (int64_t j = 0; j < actual->n_elements; j++)
		{
			
			assert_in_set((uint64_t)actual->elements[j], (uint64_t*) expected->elements, expected->n_elements);
		}
	}

	free(actual->elements);
	free(actual);
	engine_cleanup(q_h);
	teardown_example_properties(props);
}

static void ex1_test_find_all_reposts_0(void** state)
{
	ex1_test_find_all_reposts((void**) 0);
}

static void ex1_test_find_all_reposts_1_smaller(void** state)
{
	ex1_test_find_all_reposts((void**) 1);
}

static void teardown_posts(post * posts, size_t size)
{
	if (!posts)
	{
		LOG_E("Null pointer given! %c", ' ');
		return;
	}
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

static post * make_repost(post * posts, uint64_t total_posts, post * parent,  uint64_t child_idx, uint64_t max_children)
{
	if (!posts || !parent)
	{
		LOG_E("NULL pointer on array or parent given %c", '!');
		return NULL;
	}
	if (child_idx >= total_posts)
	{
		LOG_E("Child index >= total_posts, %lu >= %lu", child_idx, total_posts);
		return NULL;
	}
	if (parent->n_reposted >= max_children)
	{
		LOG_E("Too many children for parent of id %lu, setting child of idx %lu (max is %lu, have %lu", parent->pst_id, child_idx, max_children, parent->n_reposted);
		return NULL;
	}
	parent->reposted_idxs[parent->n_reposted++] = child_idx;
	return &(posts[child_idx]);
}

static void teardown_example_properties(ex_props_t * ex)
{
	teardown_posts(ex->posts, ex->n_posts);
	teardown_users(ex->users, ex->n_users);
	teardown_sample(&(ex->find_reposts));
	free(ex);
}

static void teardown_users(user * users, size_t n_users)
{
	if (!users)
	{
		LOG_E("Null pointer given %c", '!');
		return;
	}
	LOG_W("Not yet implemented %c", '!');
}

static void teardown_sample(io_sample_t * s)
{
	size_t i = 0;
	for (i = 0; i < s->n_examples; i++)
	{
		free(s->expected_outputs[i].elements);

	}
	free(s->expected_outputs);
	free(s->input_ids);
}