#ifndef CONSTS_H
#define CONSTS_H
#include "supergraph.h"
#include "common/log.h"
#include "common/dll/DLL.h"

typedef struct work_args work_args_t;
typedef struct next_of_kin kin_t;

typedef void * (*worker_f)(void*);

typedef kin_t * (*element_getter_f)(void*, uint64_t);

typedef uint8_t (*manip_elem_f)(work_args_t*,kin_t*);


void* BFS_work(void* args);

/**
 * An argument for passing to thread workers that allows parallel partiioning for
 * thread searching
 * Note: There's no need to synchronise done_flag!
 */
struct work_args
{
	size_t hi;
	size_t lo;
	void * arr;
	uint64_t * want;
	uint64_t n_want;
	uint64_t * found_idxs;
	uint8_t * found_flags;
	
	uint8_t * done_flag;
	result * result;
	
	uint64_t idx;

	dll_t ** qs;
	uint8_t n_qs;

	uint64_t * pred;
	uint8_t * bfs_flags;

	manip_elem_f manipulate_adjacent;
	manip_elem_f manipulate_elem;
	element_getter_f get_elem;
};

struct next_of_kin
{
	void * self;
	uint64_t n_out;
	uint64_t * idxs_out;
	uint64_t n_in;
	uint64_t * idxs_in;
	uint64_t id;
	uint64_t idx;
	uint64_t add_flag;
};

result * find_all_reposts_wrapper(post* posts, size_t count, uint64_t post_id, query_helper* helper);



/**
 * Exposed wrapper for find_original
 * @param posts: Array of post members to search through
 * @param count: Number of posts within array
 * @param post_id: the id of the post to be returned in result
 * @param query_helper: The query helper object
 * @return Result, with pointer to original element, and size 1, or null elements and size 0 if id not found.
 */
result * find_original_wrapper(post* posts, size_t count, uint64_t post_id, query_helper* q_h);

result* shortest_user_link_wrapper(user* users, size_t count, uint64_t userA, uint64_t userB, query_helper* helper);

work_args_t * init_work_args(void * arr, uint64_t lo, uint64_t hi, uint64_t * want, uint64_t n_want);
void destroy_work_args(work_args_t * args);

/**
 * 
 * @param search_args: Pointer to search_args_t structure, wrapping index range parameter
 */
 void* work_on_segment(void* work_args_in);

/**
 * User/Post check: 
 * Try to determine if the elem at that position is the correct one, if so, broadvast that idx 
 */

kin_t * post_getter(void* arr, uint64_t index);
kin_t * user_getter(void* arr, uint64_t index);
uint8_t generic_manip_adj(work_args_t * args, kin_t * elem);
#endif
