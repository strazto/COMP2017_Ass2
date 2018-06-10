#include "test.h"
/**
 * @author mstr3336, Matthew Strasiotto
 * Test suite for the comp2017 supergraph assignment.
 * As the repository license should dictate, free for use by anyone, but don't be silly and 
 * forget to credit if you submit this. 
 * Also don't submit this, since it wont compile on Ed
 * https://github.com/mstr3336/comp2017_supergraph2048_test.git
 */


//Static Member Functions
static void post_query_test_helper(uint64_t id_input, result * expected, void ** state, result* (query)(post*, size_t, uint64_t, query_helper*));
static void compare_results_test_helper(result * expected, result * actual);
//Find all reposts testing
static void find_all_reposts_test_helper(uint64_t id_input, result * expected, void ** state);
static void ex1_test_find_all_reposts_0_root(void** state);
static void ex1_test_find_all_reposts_1_subtree(void** state);
static void ex1_test_find_all_reposts_2_leaf(void** state);
static void ex1_test_find_all_reposts_3_not_exist(void** state);
//Find original testing
static void find_orig_test_helper(uint64_t id_input, result * expected, void ** state);
static void test_find_orig_1_for_orig(void** state);
static void test_find_orig_2_for_child(void** state);
static void test_find_orig_3_for_missing(void** state);
static void test_find_orig_4_for_grandchild(void** state);
static void test_find_orig_5_for_single(void** state);
//Shortest path testing
static void shortest_user_link_test_helper(uint64_t from, uint64_t to, result * expected, void ** state);
static void test_path_example_1_unidir_triv(void** state);
static void test_path_example_1_unidir_longer(void** state);
static void test_path_example_1_unidir_options(void** state);

//Private helpers
static int example_1_posts(void ** state);
static post * make_repost(post * posts, uint64_t total_posts, post * parent, uint64_t child_idx, uint64_t max_children);

static int shortest_path_example_1(void ** state);

int main (void)
{
	const struct CMUnitTest find_reposts_tests[]= 
	{
		cmocka_unit_test(ex1_test_find_all_reposts_0_root),
		cmocka_unit_test(ex1_test_find_all_reposts_1_subtree),
		cmocka_unit_test(ex1_test_find_all_reposts_2_leaf),
		cmocka_unit_test(ex1_test_find_all_reposts_3_not_exist)
	};
	const struct CMUnitTest find_original_tests[]=
	{
		cmocka_unit_test(test_find_orig_1_for_orig),
		cmocka_unit_test(test_find_orig_2_for_child),
		cmocka_unit_test(test_find_orig_3_for_missing),
		cmocka_unit_test(test_find_orig_4_for_grandchild),
		cmocka_unit_test(test_find_orig_5_for_single)
	};
	const struct CMUnitTest shortest_user_link_tests[]=
	{
		cmocka_unit_test(test_path_example_1_unidir_triv),
		cmocka_unit_test(test_path_example_1_unidir_longer),
		cmocka_unit_test(test_path_example_1_unidir_options)
	};
	cmocka_run_group_tests_name("FIND_ALL_REPOSTS", find_reposts_tests, example_1_posts, teardown_example_properties);
	cmocka_run_group_tests_name("FIND_ORIGINAL", find_original_tests, example_1_posts, teardown_example_properties);
	cmocka_run_group_tests_name("SHORTEST_PATH", shortest_user_link_tests, shortest_path_example_1, teardown_example_properties);
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
	user * u = NULL;
	out->posts = posts;
	out->n_posts = n_posts;
	out->n_users = 0;
	out->users = NULL;


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



static void compare_results_test_helper(result * expected, result * actual)
{
	LOG_D("Check expected element size: %lu vs actual %lu", expected->n_elements, actual->n_elements);
	assert_int_equal(expected->n_elements, actual->n_elements);

	for (int64_t j = 0; j < actual->n_elements; j++)
	{
		assert_in_set((uint64_t)actual->elements[j], (uint64_t*) expected->elements, expected->n_elements);
	}
}

static void post_query_test_helper(uint64_t id_input, result * expected, void ** state, result* (query)(post*, size_t, uint64_t, query_helper*))
{
	ex_props_t * props = *((ex_props_t**)state);
	query_helper * q_h = engine_setup(8);

	post * posts = props->posts;
	size_t n_posts = props->n_posts;
	result * actual = NULL;

	LOG_D("Testing with %lu", id_input);
	
	actual = query(posts, n_posts, id_input, q_h);	
	compare_results_test_helper(expected, actual);

	//Teardown
	free(actual->elements);
	free(actual);
	engine_cleanup(q_h);
}


static void find_all_reposts_test_helper(uint64_t id_input, result * expected, void ** state)
{
	post_query_test_helper(id_input, expected, state, find_all_reposts);
}

static void find_orig_test_helper(uint64_t id_input, result * expected, void ** state)
{
	post_query_test_helper(id_input, expected, state, find_original);
}

static void test_find_orig_1_for_orig(void** state)
{
	ex_props_t * properties = *((ex_props_t**)state);
	post * posts = properties->posts;
	uint64_t id_in = 2;
	post * elems[1] = {&posts[id_in]};
	result expected = {.elements = (void**)elems, .n_elements = 1};
	find_orig_test_helper(id_in, &expected, state);
}

static void test_find_orig_2_for_child(void** state)
{
	ex_props_t * properties = *((ex_props_t**)state);
	post * posts = properties->posts;
	uint64_t id_in = 4;
	post * elems[1] = {&posts[2]};
	result expected = {.elements = (void**)elems, .n_elements = 1};
	find_orig_test_helper(id_in, &expected, state);	
}

static void test_find_orig_3_for_missing(void** state)
{
	ex_props_t * properties = *((ex_props_t**)state);
	post * posts = properties->posts;
	uint64_t id_in = (uint64_t) -100;
	result expected = {.elements = NULL, .n_elements = 0};
	find_orig_test_helper(id_in, &expected, state);		
}

static void test_find_orig_4_for_grandchild(void** state)
{
	ex_props_t * properties = *((ex_props_t**)state);
	post * posts = properties->posts;
	uint64_t id_in = 13;
	post * elems[1] = {&posts[2]};
	result expected = {.elements = (void**)elems, .n_elements = 1};
	find_orig_test_helper(id_in, &expected, state);	
}

static void test_find_orig_5_for_single(void** state)
{
	ex_props_t * properties = *((ex_props_t**)state);
	post * posts = properties->posts;
	uint64_t id_in = 1;
	post * elems[1] = {&posts[id_in]};
	result expected = {.elements = (void**)elems, .n_elements = 1};
	find_orig_test_helper(id_in, &expected, state);	
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



static int shortest_path_example_1(void ** state)
{
	char path[] = TEST_FILE_DIR ## "PATH_EXAMPLE_1";
	ex_props_t * out = read_example(path);

	*state = (void*) out;
}


static void test_path_example_1_unidir_triv(void** state)
{
	ex_props_t * props = (ex_props_t *) *state;
	user * users = props->users;
	//Starting from elem 2

	result * expect = calloc(1,sizeof(result));
	expect->n_elements = 2;
	expect->elements = malloc(sizeof(void*)*expect->n_elements);
	void ** elements = expect->elements;
	elements[0] = &users[2];
	elements[1] = &users[1];
	shortest_user_link_test_helper(users[2].user_id, users[0].user_id, expect, state);

	free(expect);
	free(expect->elements);
	expect = NULL;

}
static void test_path_example_1_unidir_longer(void** state)
{
	ex_props_t * props = (ex_props_t *) *state;
	user * users = props->users;
	uint64_t start_from = 7;
	
	int i = 0;

	result * expect = calloc(1,sizeof(result));
	expect->n_elements = 4;
	expect->elements = malloc(sizeof(void*)*expect->n_elements);
	void ** elements = expect->elements;
	elements[i++] = &users[start_from];
	elements[i++] = &users[4];
	elements[i++] = &users[2];
	elements[i++] = &users[0];
	shortest_user_link_test_helper(users[start_from].user_id, users[0].user_id, expect, state);

	free(expect->elements);
	free(expect);

	expect = NULL;
}
static void test_path_example_1_unidir_options(void** state)
{
	ex_props_t * props = (ex_props_t *) *state;
	user * users = props->users;
	uint64_t start_from = 8;
	
	int i = 0;

	result * expect = calloc(1,sizeof(result));
	expect->n_elements = 7;
	expect->elements = malloc(sizeof(void*)*expect->n_elements);
	void ** elements = expect->elements;
	elements[i++] = &users[start_from];
	elements[i++] = &users[5];
	elements[i++] = &users[6];
	elements[i++] = &users[7];
	elements[i++] = &users[4];
	elements[i++] = &users[2];
	elements[i++] = &users[0];
	shortest_user_link_test_helper(users[start_from].user_id, users[0].user_id, expect, state);

	free(expect->elements);
	free(expect);

	expect = NULL;
}

static void shortest_user_link_test_helper(uint64_t from, uint64_t to, result * expected, void ** state)
{
	ex_props_t * props = (ex_props_t *) *state;
	user * users = props->users;
	size_t n_users = props->n_users;
	result * actual = NULL;

	query_helper * q_h = engine_setup(1);

	actual = shortest_user_link(users, n_users, from, to, q_h);
	compare_results_test_helper(expected, actual);

	free(actual->elements);
	free(actual);
	engine_cleanup(q_h);
}