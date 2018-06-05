#ifndef PARSE_CSV_H
#define PARSE_CSV_H
#include "supergraph.h"
#include "test.h"
#include "log.h"
#include <csv.h>
#include <string.h>

#define BYTES_TO_BUFF (1024)

#define N_POST_COL ("N_POSTS")
#define N_USER_COL ("N_USERS")
#define SUM_COL ("SUM")
#define ID_COL ("IDS")
typedef struct csv_environment csv_env_t;
typedef struct csv_parser csv_parse_t;
typedef void (*field_cb_f)(void*, size_t, void*);
typedef void (*row_cb_f)(int, void*);

typedef enum table_type table_type_t;

enum table_type 
{
	HEAD_TABLE,
	POST_POST,
	USER_USER,
	USER_POST
};


struct csv_environment 
{
	struct csv_parser * csv;

	table_type_t type;

	ex_props_t * properties;
	uint64_t * index;
	uint64_t index_len;

	int64_t first_user_col_idx;	
	uint64_t * cols;
	uint64_t n_cols;

	int64_t n_user_col_idx;
	int64_t n_post_col_idx;

	int64_t sum_col_idx;
	int64_t id_col_idx;

	uint64_t * sum_col;
	uint64_t * id_col;

	uint64_t current_col;
	uint64_t current_row;
};

char * buffer_in_file(char * file_path, size_t * file_size);

csv_env_t * init_from_header(void * header_buff, size_t header_size);

void ret_field(void* out, size_t bytes, void* csvenv);

void ret_field(void* out, size_t bytes, void* csvenv);

//void read_column_names(void* fbuff, size_t fsize, csv_env_t * env, field_cb_f callback_fn);

void read_any(void * data, size_t n_chars, void * csvenv);

void header_col(void * data, size_t n_chars, void * csvenv);
void normal_col(void * data, size_t n_chars, void * csvenv);
void   next_row(int charcode, void * csvenv);

void col_read_wrapper(void * data, size_t n_chars, void *csvenv);
void field_wrapper(void * data, size_t n_chars, void *csvenv);
void header_field(void * data, size_t n_chars, void *csvenv);
void normal_field(void * data, size_t n_chars, void *csvenv);

void read_matrix(void * fbuff, size_t n_bytes, csv_env_t * env, table_type_t type);

csv_env_t * init_env(ex_props_t * props, uint64_t n_cols, table_type_t type, csv_parse_t * parser);

#endif