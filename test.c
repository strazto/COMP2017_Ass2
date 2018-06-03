#include "test.h"
#include <stdlib.h>
#include <stdio.h>

//Static Member Functions
static ex_props_t * example_1_posts();
static void ex1_test_find_all_reposts(void**state);
static void ex1_test_find_all_reposts_0(void** state);
static void ex1_test_find_all_reposts_1_smaller(void** state);
static void ex1_test_find_all_reposts_2_singular(void** state);
static void ex1_test_find_all_reposts_3_NA(void** state);

//Private helpers
static post * make_repost(post * posts, uint64_t total_posts, post * parent, uint64_t child_idx, uint64_t max_children);
static void teardown_posts(post * posts, size_t n_posts);
static void teardown_example_properties(ex_props_t * ex);
static void teardown_users(user * users, size_t n_users);
static void teardown_sample(io_sample_t * s, int64_t n_examples);

int main (void)
{
	const struct CMUnitTest tests[]= 
	{
		cmocka_unit_test(ex1_test_find_all_reposts_0),
		cmocka_unit_test(ex1_test_find_all_reposts_1_smaller),
		cmocka_unit_test(ex1_test_find_all_reposts_2_singular),
		cmocka_unit_test(ex1_test_find_all_reposts_3_NA)
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
	size_t ex_idx = 0;
	io_sample_t * s;

	for (i = 0; i < n_posts; i++)
	{
		posts[i].pst_id = i;
		LOG_V("Setting id of:%lu to post idx: %ld", posts[i].pst_id, i);
	}


	//Do find reposts
	out->n_repost_examples = 4;
	out->find_reposts = calloc(out->n_repost_examples, sizeof(io_sample_t));
	
	ex = out->find_reposts;

	ex[ex_idx].input_val = posts[2].pst_id;
	ex[ex_idx].res.n_elements = 9;
	ex[ex_idx].res.elements = malloc(ex[ex_idx].res.n_elements*sizeof(post*));
	post ** elems = (post**) ex[ex_idx].res.elements;

	i = 0;


	p = &posts[2];
	elems[i++] = p;
	p->n_reposted = 0;
	max_children = 5;
	p->reposted_idxs = malloc(sizeof(size_t)*max_children);
	
	elems[i++] = make_repost(posts, n_posts, p, 4, max_children);
	elems[i++] = make_repost(posts, n_posts, p, 6, max_children);
	elems[i++] = make_repost(posts, n_posts, p, 7, max_children);
	elems[i++] = make_repost(posts, n_posts, p, 10, max_children);
	elems[i++] = make_repost(posts, n_posts, p, 11, max_children);
		
	p = &posts[7];
	max_children = 3;

	p->reposted_idxs = malloc(sizeof(size_t)*max_children);

	elems[i++] = make_repost(posts, n_posts, p, 15, max_children);
	elems[i++] = make_repost(posts, n_posts, p, 17, max_children);
	elems[i++] = make_repost(posts, n_posts, p, 13, max_children);

	ex[ex_idx].res.n_elements = i;

	s = &ex[++ex_idx];

	s->input_val = posts[7].pst_id;
	max_children = 3;
	s->res.n_elements = 4;
	s->res.elements = calloc(s->res.n_elements, sizeof(post*));
	

	i = 0;
	elems = (post**) s->res.elements;
	elems[i++] = &posts[7];
	elems[i++] = &posts[15];
	elems[i++] = &posts[17];
	elems[i++] = &posts[13];

	//EXAMPLE 3
	s = &ex[++ex_idx];
	s->input_val = posts[1].pst_id;
	s->res.n_elements = 1;
	s->res.elements = malloc(sizeof(post*)*s->res.n_elements);
	
	elems = (post**) s->res.elements;
	elems[0] = &posts[1];

	//Example 4
	s = &ex[++ex_idx];
	s->input_val = (uint64_t) -10;

	s->res.n_elements = 0;
	s->res.elements = NULL;

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

	io_sample_t * ex = props->find_reposts;


	if (i < props->n_repost_examples)
	{
		LOG_D("Starting Test %ld", i);
		expected = &ex[i].res;
		actual = find_all_reposts(posts, n_posts, ex[i].input_val, q_h);

		
		LOG_D("Check the two elements!%c", '!');
		assert_int_equal(expected->n_elements, actual->n_elements);

		for (int64_t j = 0; j < actual->n_elements; j++)
		{
			
			assert_in_set((uint64_t)actual->elements[j], (uint64_t*) expected->elements, expected->n_elements);
		}
	} else LOG_E("Invalid test data %c", '!');

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

static void ex1_test_find_all_reposts_2_singular(void** state)
{
	ex1_test_find_all_reposts((void**) 2);
}

static void ex1_test_find_all_reposts_3_NA(void** state)
{
	ex1_test_find_all_reposts((void**) 3);
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
	teardown_sample(ex->find_reposts, ex->n_repost_examples);
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

static void teardown_sample(io_sample_t * s, int64_t n_examples)
{
	size_t i = 0;
	for (i = 0; i < n_examples; i++)
	{
		if (s[i].res.elements) free(s[i].res.elements);
	}
	free(s);
}