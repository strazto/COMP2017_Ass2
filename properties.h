#ifndef PROPERTIES_H
#define PROPERTIES_H

#include "supergraph.h"

typedef struct example_properties ex_props_t;

struct example_properties
{
	post * posts;
	size_t n_posts;
	user * users;
	size_t n_users;


};


void teardown_posts(post * posts, size_t n_posts);
int teardown_example_properties(void ** state);
void teardown_users(user * users, size_t n_users);

#endif