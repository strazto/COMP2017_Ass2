#ifndef TEST_H
#define TEST_H
#include "supergraph.h"
//#include "mine.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include "log.h"

typedef struct example_properties ex_props_t;
typedef struct input_output_examples io_sample_t;




struct input_output_examples 
{
	int64_t input_val;
	result  res;
};


struct example_properties
{
	post * posts;
	size_t n_posts;
	user * users;
	size_t n_users;

	size_t n_repost_examples;
	io_sample_t * find_reposts;
};


#endif
