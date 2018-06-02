#ifndef TEST_H
#define TEST_H
#include "supergraph.h"
#include "mine.h"
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include "log.h"

typedef struct example_properties ex_props_t;
typedef struct input_output_examples io_sample_t;

static ex_props_t * example_1_posts();
static void teardown_posts();
static void ex1_test_find_all_reposts(void**state);



struct example_properties
{
	post * posts;
	size_t n_posts;
	user * users;
	size_t n_users;

	io_sample_t find_reposts;
};

struct input_output_examples 
{
	size_t n_examples;
	int64_t * input_ids;
	result * expected_outputs;
};

#endif
