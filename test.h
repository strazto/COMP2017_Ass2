#ifndef TEST_H
#define TEST_H
#include "supergraph.h"
#include "mine.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include "log.h"

typedef struct example_properties ex_props_t;
typedef struct input_output_examples io_sample_t;

static ex_props_t * example_1_posts();
static void teardown_posts(post * posts, size_t n_posts);
static void ex1_test_find_all_reposts(void**state);
static void ex1_test_find_all_reposts_0(void** state);
static void ex1_test_find_all_reposts_1_smaller(void** state);
static void ex1_test_find_all_reposts_2_singular(void** state);
static void ex1_test_find_all_reposts_3_NA(void** state);

struct input_output_examples 
{
	size_t n_examples;
	int64_t * input_ids;
	result * expected_outputs;
};


struct example_properties
{
	post * posts;
	size_t n_posts;
	user * users;
	size_t n_users;

	io_sample_t find_reposts;
};

static post * make_repost(post * posts, uint64_t total_posts, post * parent, uint64_t child_idx, uint64_t max_children);

static void teardown_example_properties(ex_props_t * ex);
static void teardown_users(user * users, size_t n_users);
static void teardown_sample(io_sample_t * sample);
#endif
