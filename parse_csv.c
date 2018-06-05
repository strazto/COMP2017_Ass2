#include "parse_csv.h"

typedef struct csv_parser csv_parse_t;



char * buffer_in_file(char * file_path, size_t * file_size)
{
	(void) n_chars_path;
	char * out = NULL;
	size_t n_read = 0;	

	//Find the size of the header
	FILE * f = fopen(header_path, "rb");
	fseek(fp, 0L, SEEK_END);
	*file_size = ftell(fp);
	rewind(f);

	LOG_I("File size: %lu", header_size);
	
	out = malloc(*file_size);

	while (!feof(f) && n_read < *file_size)
	{
		n_read += fread(out, 1, *file_size, f);
	}

	LOG_I("Number of bytes read: %lu", n_read);

	fclose(f);
	return out;
}


ex_props_t * init_from_header(void * header_buff, size_t header_size)
{
	csv_env_t env = 
	{
		.properties = calloc(1,sizeof(ex_props_t)),
		.n_cols = 3,
		.current_col = 0,
		.current_row = 0,
		.first_user_col_idx = -1,
		.is_header = 1
	}
	csv_parse_t * csv;
	csv_init(csv, CSV_APPEND_NULL);
	env.csv = csv;

	csv_parse(csv, header_buff, header_size, read_any, next_row, &env);
	csv_fini(csv, read_any, next_row, &env)
	
	return NULL
}

field_cb_f read_any(void * data, size_t n_chars, void * csvenv)
{
	csv_env_t * env = (csv_env_t *) csvenv;
	field_cb_f cb = field_wrapper;
	if (env->current_row == 0) 	cb = col_read_wrapper;

	cb(data, n_chars, csvenv);
					
}

field_cb_f col_read_wrapper(void * data, size_t n_chars, void *csvenv)
{
	csv_env_t * env = (csv_env_t *) csvenv;

	field_cb_f cb = normal_col;
	
	if (env->is_header) cb = header_col;
	cb(data, n_chars, csvenv);
}

field_cb_f header_col(void * data, size_t n_chars, void * csvenv)
{
	csv_env_t * env = (csv_env_t *) csvenv;
			if (!strncmp(data, N_POST_COL, 	n_chars)) env->n_pos_col_idx	= env->current_col;
	else 	if (!strncmp(data, N_USER_COL, 	n_chars)) env->n_user_col_idx	= env->current_col;
}

field_cb_f normal_col(void * data, size_t n_chars, void * csvenv)
{
	csv_env_t * env = (csv_env_t*) csvenv;

			if (!strncmp(data, SUM_COL,		n_chars)) env->sum_col_idx		= env->current_col;
	else	if (!strncmp(data, ID_COL,		n_chars)) env->id_col_idx		= env->current_col;
	else 
	{
		if (env->first_user_col_idx < 0) env->first_user_col_idx = env->current_col;

		sscanf(data,"%lu", &env->cols[env->current_col - env->first_user_col_idx]);
	}
}

field_cb_f field_wrapper(void * data, size_t n_chars, void *csvenv)
{
	csv_env_t * env = (csv_env_t *) csvenv;


	field_cb_f cb = normal_field;
	if (env->is_header) cb = header_field;

	cb(data, n_chars, csvenv);
}


row_cb_f next_row(int char, void * csvenv)
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