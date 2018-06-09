#include "mine.h"

typedef struct work_args work_args_t;
typedef enum array_type array_type_t;

/**
 * param:: void * array
 * 		:: uint64_t index to look at
 * 		:: uint64_t desired id
 * 		:: uint8_t * done flag
 * 		:: uint64_t * 
 * 
 */
typedef void * (*worker_f)(void*, uint64_t, uint64_t, uint8_t*, uint64_t*);

/////////////////////////////////////////////////////////////
//				Static Functions (Private)
/////////////////////////////////////////////////////////////


static work_args_t * init_work_args(void * arr, uint64_t lo, uint64_t hi, uint64_t want, uint64_t * result, uint8_t * done_flag);


/**
 * 
 * @param search_args: Pointer to search_args_t structure, wrapping index range parameter
 */
static void* work_on_segment(void* work_args_in);


/**
 * User/Post check: 
 * Try to determine if the user at that position is the correct one, if so, broadvast that idx 
 * 
 * @param arr: 			the array,
 * @param idx: 			the index to check
 * @param want:			the value to find
 * @param done_flag:	the flag indicating done-ness (for // threads to terminate on)
 * @param result:		put the resulting index value here
 */
static void* post_check(void* arr, uint64_t idx, uint64_t want, uint8_t * done_flag, uint64_t * result);
static void* user_check(void* arr, uint64_t idx, uint64_t want, uint8_t * done_flag, uint64_t * result);
/////////////////////////////////////////////////////////////
//				Helper data classes
/////////////////////////////////////////////////////////////

// enum array_type 
// {
// 	POST_ARR = 0,
// 	USER_ARR = 1,
// 	INDX_ARR = 2
// };

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
	uint64_t id;
	uint8_t * done_flag;
	uint64_t * result;
	worker_f work;
};





/*
 * Find the location of an id from within an array
 * :: void* work_args :: 
 * Returns id
 */
static void* work_on_segment(void* work_args_in)
{
	work_args_t * args = (work_args_t *) work_args_in;
	uint64_t i = 0;
	for (i = args->lo; i < args->hi && !args->done_flag[0]; i++)
	{
		args->work(args->arr, i, args->id, args->done_flag, args->result);
		if (args->done_flag[0]) LOG_D("Work is done, @ %lu", i);
	}
	return (void*) args->result;
}

result* find_all_reposts_wrapper(post* posts, size_t count, uint64_t post_id, query_helper* helper) 
{
	//FInd the thing, and enqueue its babies
	//Could try selectively recursing to some depth
	uint64_t i = 0;
	uint64_t j = 0;
	post * current = NULL;
	uint64_t * idx_result = NULL;
	uint8_t * done_flag = NULL;
	work_args_t * wargs = NULL;
	dll_t * q = NULL;
	dll_t * out_buff = NULL;
	result * out = NULL;
	

	out = calloc(1,sizeof(result));
	//Find the index
	idx_result = malloc(sizeof(uint64_t));
	done_flag = calloc(1,sizeof(uint8_t));	
	wargs = init_work_args((void*)posts, 0, count, post_id, idx_result, done_flag);
	//The current work is to check for an index!
	wargs->work = post_check;
	
	work_on_segment((void*) wargs);
	if (!wargs->done_flag[0])
	{
		LOG_D("Never found original, cleaning up and LEAVING %c", '!');
		free(idx_result);
		idx_result = NULL;
		free(done_flag);
		done_flag = NULL;
		free(wargs);
		wargs = NULL;
		return out;
	}
		
	
	q = dll_init();
	out_buff = dll_init();
	//Queue its children
	dll_enqueue(q, idx_result);
	//Pop its children
	while (q->size)
	{
		i = *(uint64_t *) dll_pop(q);		
		current = &posts[i];
		dll_enqueue(out_buff, current);
		for (j = 0; j < current->n_reposted; j++)
		{
			dll_enqueue(q, &current->reposted_idxs[j]);
		}
	}
	
	out->n_elements = out_buff->size;
	out->elements = malloc(sizeof(void*)*out->n_elements);
	LOG_D("Found %lu reposts!", out->n_elements);
	for (i = 0; i < out->n_elements && out_buff->size; i++)
	{
		out->elements[i] = dll_pop(out_buff);
	}
	dll_destroy(q);
	dll_destroy(out_buff);
	free(idx_result);
	free(done_flag);
	free(wargs);

	return out;
}



result * find_original_wrapper(post* posts, size_t count, uint64_t post_id, query_helper* q_h)
{
	//For me, you want to go through the array (May partition)
	return NULL;
}


static work_args_t * init_work_args(void * arr, uint64_t lo, uint64_t hi, uint64_t want, uint64_t * result, uint8_t * done_flag)
{
	work_args_t * out = malloc(sizeof(work_args_t));
	out->hi = hi;
	out->lo = lo;
	out->arr = arr;
	out->id = want;

	out->done_flag = done_flag;
	out->result = result;
	return out;
}


static void* post_check(void* arr, uint64_t idx, uint64_t want, uint8_t * done_flag, uint64_t * result)
{
	post * posts = (post *) arr;
	if (posts[idx].pst_id == want)
	{
		LOG_D("Found the desired id (%lu) @ %lu",want, idx);
		result[0] = idx;
		done_flag[0]++;
	}
	return (void*) result;
}

static void* user_check(void* arr, uint64_t idx, uint64_t want, uint8_t * done_flag, uint64_t * result)
{
	user * users = (user *) arr;
	if (users[idx].user_id == want)
	{
		LOG_D("Found the desired id (%lu) @ %lu",want, idx);
		result[0] = idx;
		done_flag[0]++;
	}
	return (void*) result;
}