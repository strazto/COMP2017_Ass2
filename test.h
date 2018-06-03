#ifndef TEST_H
#define TEST_H
#include "supergraph.h"
//#include "mine.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "log.h"

typedef struct example_properties ex_props_t;

struct example_properties
{
	post * posts;
	size_t n_posts;
	user * users;
	size_t n_users;


};


#endif
