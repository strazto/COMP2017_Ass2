#include "parse_csv.h"





char * buffer_in_file(char * file_path, size_t * file_size)
{
	char * out = NULL;
	size_t n_read = 0;	

	LOG_I("Buffering in file: %s", file_path);

	//Find the size of the header
	FILE * f = fopen(file_path, "rb");
	if (!f)
	{
		LOG_E("Error opening %s, terminating.", file_path);
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	*file_size = ftell(f);
	rewind(f);

	LOG_I("File size: %lu", *file_size);
	
	out = malloc(*file_size);

	while (!feof(f) && n_read < *file_size)
	{
		n_read += fread(out, 1, *file_size, f);
	}

	LOG_I("Number of bytes read: %lu", n_read);

	fclose(f);
	return out;
}


csv_env_t * init_from_header(void * header_buff, size_t header_size)
{
	csv_env_t * env = init_env(malloc(sizeof(ex_props_t)), 3, HEAD_TABLE, NULL);
	csv_parse(env->csv, header_buff, header_size, read_any, next_row, env);
	csv_fini(env->csv, read_any, next_row, env);

	env->pp_tracker = calloc(env->properties->n_posts, sizeof(uint64_t));
	env->up_tracker = calloc(env->properties->n_users, sizeof(uint64_t));
	env->uu_tracker = calloc(env->properties->n_users, sizeof(uint64_t));

	return env;
}

void read_matrix(void * fbuff, size_t n_bytes, csv_env_t * env, table_type_t type)
{
	env->type = type;
	env->current_row = 0;
	env->current_col = 0;

	csv_parse(env->csv, fbuff, n_bytes, read_any, next_row, env);
	csv_fini(env->csv, read_any, next_row, env);
}

csv_env_t * init_env(ex_props_t * props, uint64_t n_cols, table_type_t type, csv_parse_t * parser)
{
	csv_env_t * out = calloc(1, sizeof(csv_env_t));
	out->properties = props;
	out->n_cols = n_cols;
	out->first_user_col_idx = -1;
	out->type = type;
	
	if (!parser)
	{
		parser = malloc(sizeof(csv_parse_t));
		csv_init(parser, CSV_APPEND_NULL);
	} 
	out->csv = parser;
	return out;
}

void read_any(void * data, size_t n_chars, void * csvenv)
{
	csv_env_t * env = (csv_env_t *) csvenv;
//	LOG_V("Parsing R:%lu|C:%lu|D:%s", env->current_row, env->current_col, (char*) data);
	field_cb_f cb = field_wrapper;
	if (env->current_row == 0) 	cb = meta_row;
	if (env->current_col++ != 0) cb(data, n_chars, csvenv);
}


void meta_row(void * data, size_t n_chars, void *csvenv)
{
	csv_env_t * env = (csv_env_t *) csvenv;
	
	switch (env->type)
	{
		case HEAD_TABLE:
			if 		(!strncmp(data, N_POST_COL, n_chars)) env->n_post_col_idx	= env->current_col;
			else if (!strncmp(data, N_USER_COL, n_chars)) env->n_user_col_idx	= env->current_col;
		break;
		default:
			if 		(!strncmp(data, SUM_COL,	n_chars)) env->sum_col_idx		= env->current_col;
			else if (!strncmp(data, ID_COL,		n_chars)) env->id_col_idx		= env->current_col;
			else if (env->first_user_col_idx < 0) 		  env->first_user_col_idx = env->current_col;	
		break;
	}
}

void field_wrapper(void * data, size_t n_chars, void *csvenv)
{
	csv_env_t * env = (csv_env_t *) csvenv;
	field_cb_f cb = normal_field;
	if (env->type == HEAD_TABLE) cb = header_field;

	switch (env->type)
	{
		case HEAD_TABLE:
		cb = header_field;
		break;
		default:
		cb = normal_field;
		break;
	}

	cb(data, n_chars, csvenv);
}


void next_row(int charcode, void * csvenv)
{
	csv_env_t * env = (csv_env_t *) csvenv;
	env->current_col=0;
	env->current_row++;
}


void header_field(void * data, size_t n_chars, void *csvenv)
{
	csv_env_t * env = (csv_env_t *) csvenv;

	if (env->current_col == env->n_user_col_idx)
	{
		 sscanf(data, "%lu", &env->properties->n_users);
		 env->properties->users = calloc(env->properties->n_users, sizeof(user));
	}
	if (env->current_col == env->n_post_col_idx) 
	{
		sscanf(data, "%lu", &env->properties->n_posts);
		env->properties->posts = calloc(env->properties->n_posts, sizeof(post));
	}
}


void normal_field(void * data, size_t n_chars, void *csvenv)
{
	csv_env_t * env = (csv_env_t *) csvenv;
	ex_props_t * p = env->properties;
	uint64_t temp;
	sscanf(data, "%lu", &temp);

	//LOG_V("@R:%lu|C:%lu|VAL:%lu", env->current_row, env->current_col, temp);
	if (env->current_col == env->sum_col_idx)
	{
		sum_field(temp, env);
		return;
	}
	if (env->current_col == env->id_col_idx)
	{
		id_field(temp, env);
		return;	
	}
	
	data_field(temp, env);
	
}


void sum_field(uint64_t val, csv_env_t * env)
{
	 ex_props_t * p = env->properties;
	 uint64_t i = env->current_row - 1;
	 LOG_I("SUM_FIELD[%lu] = %lu", i, val);
	 uint64_t * arr = NULL;
	 if (val) arr = malloc(sizeof(uint64_t)*val);
	 switch (env->type)
	 {
	 	case POST_POST:
	 		p->posts[i].n_reposted = val;
	 		p->posts[i].reposted_idxs = arr;
	 	break;
	 	case USER_USER:
	 		p->users[i].n_following = val;
	 		p->users[i].following_idxs = arr;
	 	break;
	 	case USER_POST:
	 		p->users[i].n_posts = val;
	 		p->users[i].post_idxs = arr;
	 	break;
	 	default:
	 	break;
	 }	
}

void id_field(uint64_t val, csv_env_t * env)
{
	ex_props_t * p = env->properties;
	uint64_t i = env->current_row - 1;
	LOG_I("ID_FIELD[%lu] = %lu", i, val);
	switch (env->type)
	{
		case POST_POST:
			p->posts[i].pst_id = val;
			break;
		case USER_USER:
			p->users[i].user_id = val;
			break;
		default:
		break;
	}	
}

void data_field(uint64_t val, csv_env_t * env)
{
	ex_props_t * p = env->properties;
	uint64_t i = env->current_row - 1;
	if (env->first_user_col_idx < 0)
	{
		LOG_E("NO USER IDX TRACKER ASSIGNED, cant assign elem %lu", i);
	}
	uint64_t data_idx = env->current_col - env->first_user_col_idx;
	LOG_V("\tDATA_FIELD [%lu][%lu] = %lu", i, data_idx, val);
	if (val == 0) return;
	switch (env->type)
	{
		case POST_POST:
			if (env->pp_tracker[i] >= p->posts[i].n_reposted)
			{
				LOG_E("Trying to assign too many children to post idx %lu", i);
				return;
			}
			p->posts[i].reposted_idxs[env->pp_tracker[i]++] = data_idx;				
			break;
		case USER_USER:
			if (env->uu_tracker[i] >= p->users[i].n_following)
			{
				LOG_E("Trying to make user %lu follow too many!", i);
				return;
			}
			p->users[i].following_idxs[env->uu_tracker[i]++] = data_idx;
			break;
		case USER_POST:
			if (env->up_tracker[i] >= p->users[i].n_posts)
			{
				LOG_E("Trying to assign too many posts to user %lu!", i);
				return;				
			}
			p->users[i].post_idxs[env->up_tracker[i]++] = data_idx;
			break;
	}	
}


void print_post_info(post* posts, uint64_t count)
{
	uint64_t i = 0;
	uint64_t j = 0;
	for (i = 0; i < count; i++)
	{
		printf("p[%lu].id = %lu", i, posts[i].pst_id);
		printf("\tn_reposts: %lu|", posts[i].n_reposted);
		for (j = 0; j < posts[i].n_reposted; j++) printf("%lu|", posts[i].reposted_idxs[j]);
		printf("\n");
	}
}


void print_user_info(user* users, uint64_t count)
{
	uint64_t i = 0;
	uint64_t j = 0;
	for (i = 0; i < count; i++)
	{
		printf("u[%lu].id = %lu", i, users[i].user_id);
		printf("\tn_following: %lu", users[i].n_following);
		if (users[i].n_following) 
		{
			printf("\n\t");
			for (j = 0; j < users[i].n_following; j++) printf("%lu|", users[i].following_idxs[j]);
		}
		printf("\n");
		printf("n_followers: %lu", users[i].n_followers);
		if (users[i].n_followers) 
		{
			printf("\n\t");
			for (j = 0; j < users[i].n_followers; j++) printf("%lu|", users[i].follower_idxs[j]);
		}
		printf("\n");
	}
}