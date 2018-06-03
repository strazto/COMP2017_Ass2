#include "test.h"

//Static Member Functions
static void compare_results_test_helper(result * expected, result * actual);
//Find all reposts testing
static void find_all_reposts_test_helper(uint64_t id_input, result * expected, void ** state);
static void ex1_test_find_all_reposts_0_root(void** state);
static void ex1_test_find_all_reposts_1_subtree(void** state);
static void ex1_test_find_all_reposts_2_leaf(void** state);
static void ex1_test_find_all_reposts_3_not_exist(void** state);
//Find original testing
static void test_find_orig_1_for_orig(void** state);
static void test_find_orig_2_for_child(void** state);
static void test_find_orig_3_for_missing(void** state);
static void test_find_orig_4_for_single(void** state);
//Private helpers
static int example_1_posts(void ** state);
static post * make_repost(post * posts, uint64_t total_posts, post * parent, uint64_t child_idx, uint64_t max_children);
static void teardown_posts(post * posts, size_t n_posts);
static int teardown_example_properties(void ** state);
static void teardown_users(user * users, size_t n_users);

int main (void)
{
	const struct CMUnitTest find_reposts_tests[]= 
	{
		cmocka_unit_test(ex1_test_find_all_reposts_0_root),
		cmocka_unit_test(ex1_test_find_all_reposts_1_subtree),
		cmocka_unit_test(ex1_test_find_all_reposts_2_leaf),
		cmocka_unit_test(ex1_test_find_all_reposts_3_not_exist)
	};
	cmocka_run_group_tests_name("FIND_ALL_REPOSTS", find_reposts_tests, example_1_posts, teardown_example_properties);

	return 0;
}


static int example_1_posts(void ** state)
{
	
	ex_props_t * out = malloc(sizeof(ex_props_t));

	uint64_t max_children = 0;
	size_t n_posts = 20;
	uint64_t i = 0;
	post* posts = calloc(n_posts, sizeof(post));
	post* p = NULL;
	
	out->posts = posts;
	out->n_posts = n_posts;
	out->users = NULL;
	out->n_users = 0;

	for (i = 0; i < n_posts; i++)
	{
		posts[i].pst_id = i;
		LOG_V("Setting id of:%lu to post idx: %ld", posts[i].pst_id, i);
	}

	p = &posts[2];
	p->n_reposted = 0;
	max_children = 5;
	p->reposted_idxs = malloc(sizeof(size_t)*max_children);
	
	make_repost(posts, n_posts, p, 4, max_children);
	make_repost(posts, n_posts, p, 6, max_children);
	make_repost(posts, n_posts, p, 7, max_children);
	make_repost(posts, n_posts, p, 10, max_children);
	make_repost(posts, n_posts, p, 11, max_children);
		
	p = &posts[7];
	max_children = 3;

	p->reposted_idxs = malloc(sizeof(size_t)*max_children);

	make_repost(posts, n_posts, p, 15, max_children);
	make_repost(posts, n_posts, p, 17, max_children);
	make_repost(posts, n_posts, p, 13, max_children);

	*state = (void*) out;
	return 0;
}

static void ex1_test_find_all_reposts_0_root(void** state)
{
	ex_props_t * properties = *((ex_props_t**)state);
	post * p = properties->posts;
	result expected;
	uint64_t id_in = 2;

	expected.n_elements = 9;
	post * elems[9] = { &p[2], &p[4], &p[6], &p[7], &p[10], &p[11], &p[15], &p[17], &p[13] };
	expected.elements = (void **) elems;

	find_all_reposts_test_helper(id_in, &expected, state);
}

static void ex1_test_find_all_reposts_1_subtree(void** state)
{
	ex_props_t * properties = *((ex_props_t**)state);
	post * p = properties->posts;
	result expected;
	uint64_t id_in = 7;

	expected.n_elements = 4;
	post * elems[4] = { &p[7], &p[15], &p[17], &p[13] };
	expected.elements = (void **) elems;

	find_all_reposts_test_helper(id_in, &expected, state);	
}


static void ex1_test_find_all_reposts_2_leaf(void** state)
{
	ex_props_t * properties = *((ex_props_t**)state);
	post * p = properties->posts;
	result expected;
	uint64_t id_in = 1;

	expected.n_elements = 1;
	post * elems[1] = {&p[1]};
	expected.elements = (void **) elems;

	find_all_reposts_test_helper(id_in, &expected, state);	
}

static void ex1_test_find_all_reposts_3_not_exist(void** state)
{
	ex_props_t * properties = *((ex_props_t**)state);
	post * p = properties->posts;
	result expected;
	uint64_t id_in = (uint64_t) -100;

	expected.n_elements = 0;
	post ** elems = NULL;
	expected.elements = (void **) elems;

	find_all_reposts_test_helper(id_in, &expected, state);	
}

static void find_all_reposts_test_helper(uint64_t id_input, result * expected, void ** state)
{
	ex_props_t * props = *((ex_props_t**)state);
	query_helper * q_h = engine_setup(8);

	post * posts = props->posts;
	size_t n_posts = props->n_posts;
	result * actual = NULL;

	LOG_D("Testing with %lu", id_input);
	
	actual = find_all_reposts(posts, n_posts, id_input, q_h);	
	compare_results_test_helper(expected, actual);

	//Teardown
	free(actual->elements);
	free(actual);
	engine_cleanup(q_h);
}

static void compare_results_test_helper(result * expected, result * actual)
{
	LOG_D("Check expected element size: %lu vs actual %lu", expected->n_elements, actual->n_elements);
	assert_int_equal(expected->n_elements, actual->n_elements);

	for (int64_t j = 0; j < actual->n_elements; j++)
	{
		assert_in_set((uint64_t)actual->elements[j], (uint64_t*) expected->elements, expected->n_elements);
	}
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

static int teardown_example_properties(void ** state)
{
	ex_props_t * properties = *((ex_props_t**)state);
	teardown_posts(properties->posts, properties->n_posts);
	teardown_users(properties->users, properties->n_users);
	free(properties);
	return 0;
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

