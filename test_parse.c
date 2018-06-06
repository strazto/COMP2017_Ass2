#include "parse_csv.h"

#define DIR "python/TEST_OUT"
#define HEAD_PATH DIR "/HEADER.csv" 
#define PP_PATH  DIR "/POSTS.csv" 
#define UU_PATH  DIR "/USERS.csv"
#define UP_PATH  DIR "/USER_POSTS.csv"


void main(void)
{
	size_t file_size = 0;
	char * fbuff = buffer_in_file(HEAD_PATH, &file_size);
	LOG_I("Buffered in " HEAD_PATH " of size %lu", file_size);

	csv_env_t * env = init_from_header(fbuff, file_size);
	ex_props_t * props = env->properties;

	LOG_I("In initialized header info, discerned %lu users, %lu posts", props->n_users, props->n_posts);

	free(fbuff);
	fbuff = NULL;

	fbuff = buffer_in_file(PP_PATH, &file_size);
	LOG_I("Buffered in " PP_PATH "  of size %lu", file_size);
	read_matrix(fbuff, file_size, env, POST_POST);

	free(fbuff);
	fbuff = NULL;

	fbuff = buffer_in_file(UU_PATH, &file_size);
	LOG_I("Buffered in " UU_PATH " of size %lu", file_size);
	read_matrix(fbuff, file_size, env, USER_USER);

	free(fbuff);
	fbuff = NULL;

	fbuff = buffer_in_file(UP_PATH, &file_size);
	LOG_I("Buffered in " UP_PATH " of size %lu", file_size);
	read_matrix(fbuff, file_size, env, USER_POST);

	free(fbuff);
	fbuff = NULL;

	print_post_info(env->properties->posts, env->properties->n_posts);
	print_user_info(env->properties->users, env->properties->n_users);
}