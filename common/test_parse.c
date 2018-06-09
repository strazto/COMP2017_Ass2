#include "parse_csv.h"
#include "properties.h"

void main(void)
{
	char path[] = "generator/TEST_OUT";	

	ex_props_t * props = read_example(path);

	print_post_info(props->posts, props->n_posts);
	print_user_info(props->users, props->n_users);

	teardown_example_properties( (void**) &props);
}