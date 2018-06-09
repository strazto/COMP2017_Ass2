#include "mine.h"

typedef struct work_args work_args_t;
typedef struct next_of_kin kin_t;
typedef enum array_type array_type_t;

/**
 * param:: void * array
 * 		:: uint64_t index to look at
 * 		:: uint64_t desired id
 * 		:: uint8_t * done flag
 * 		:: uint64_t * 
 * 
 */
typedef void * (*worker_f)(void*);
typedef uint8_t (*comp_f)(void*, uint64_t,uint64_t);
typedef void * (*done_evaluator_f)(void*);
/////////////////////////////////////////////////////////////
//				Static Functions (Private)
/////////////////////////////////////////////////////////////


static work_args_t * init_work_args(void * arr, uint64_t lo, uint64_t hi, uint64_t * want, uint64_t n_want);
static void destroy_work_args(work_args_t * args);

/**
 * 
 * @param search_args: Pointer to search_args_t structure, wrapping index range parameter
 */
static void* work_on_segment(void* work_args_in);
static void* BFS_work(void* args);

static void * find_original_worker(void* worker_args);

static void * find_reposts_worker(void* worker_args);
/**
 * User/Post check: 
 * Try to determine if the elem at that position is the correct one, if so, broadvast that idx 
 */
static void* id_check(void* worker_args);
static uint8_t post_comp(void* arr, uint64_t want, uint64_t idx);
static uint8_t user_comp(void* arr, uint64_t want, uint64_t idx);

static void * done_if_found_one(void* wargs);
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
	uint64_t * want;
	uint64_t n_want;
	uint64_t * found_idxs;
	uint8_t * found_flags;
	
	uint8_t * done_flag;
	result * result;
	
	uint64_t idx;

	dll_t * q;
	dll_t * out_buff;

	uint64_t * parents;
	uint8_t * bfs_flags;

	worker_f work;
	comp_f comp;
	done_evaluator_f check_done;
};

struct next_of_kin
{
	void * self;
	uint64_t n_children;
	uint64_t * idxs;
};


/*
 * Find the location of an id from within an array
 * :: void* work_args :: 
 * Returns id
 */
static void* work_on_segment(void* work_args_in)
{
	work_args_t * args = (work_args_t *) work_args_in;
	for (args->idx = args->lo; args->idx < args->hi && !args->done_flag[0]; args->idx++)
	{
		if (args->work) args->work(work_args_in);
		if (args->comp) id_check(work_args_in);
		if (args->check_done) args->check_done(work_args_in);
		if (args->done_flag[0]) LOG_D("Work is done, @ %lu", args->idx);
	}
	return (void*) args->result;
}

result* find_all_reposts_wrapper(post* posts, size_t count, uint64_t post_id, query_helper* helper) 
{
	//FInd the thing, and enqueue its babies
	//Could try selectively recursing to some depth
	uint64_t * idx_result = NULL;
	work_args_t * wargs = NULL;
	result * out = calloc(1,sizeof(result));
	
	uint64_t * want = malloc(sizeof(uint64_t));
	*want = post_id;


	
	//Find the index
	wargs = init_work_args((void*)posts, 0, count, want, 1);
	wargs->result = out;
	//The current work is to check for an index!
	wargs->work = id_check;
	wargs->comp = post_comp;
	wargs->check_done = done_if_found_one;

	

	work_on_segment((void*) wargs);
	if (!wargs->done_flag[0])
	{
		LOG_D("Never found original, cleaning up and LEAVING %c", '!');
		destroy_work_args(wargs);
		wargs = NULL;
		return out;
	}
	LOG_D("Discerned index: %lu", wargs->found_idxs[0]);
	*wargs->done_flag = 0;	

	wargs->idx = wargs->found_idxs[0];
	wargs->work = find_reposts_worker;
	wargs->check_done = NULL;
	wargs->comp = NULL;
	wargs->q = dll_init();
	wargs->out_buff = dll_init();

	BFS_work((void*) wargs);

	destroy_work_args(wargs);

	return out;
}

/**
 * For now, will leave this, but could consider for when Mthreading, multiple BFSs, 
 * initially for the index u want, but then maybe also
 * 
 */

result * find_original_wrapper(post* posts, size_t count, uint64_t post_id, query_helper* q_h)
{
	uint64_t i = 0;
	post * current = NULL;
	//For me, you want to go through the array (May partition)
	uint64_t * want = malloc(sizeof(uint64_t));
	*want = post_id;
	result * out = calloc(1,sizeof(result));

	work_args_t * wargs = init_work_args(posts, 0, count, want, 1);
	wargs->result = out;
	wargs->parents = malloc(sizeof(uint64_t)*count);
	wargs->bfs_flags = calloc(count, sizeof(uint8_t));
	wargs->work = find_original_worker;
	wargs->comp = post_comp;
	work_on_segment(wargs);

	if (!wargs->found_flags[0])
	{
		LOG_D("Didn't find id %lu anywhere! :(", wargs->want[0]);
		destroy_work_args(wargs);
		return out;
	}
	i = wargs->found_idxs[0];
	current = &posts[i];
	while (wargs->bfs_flags[i])
	{
		i = wargs->parents[i];
		current = &posts[i];
	}

	out->n_elements = 1;
	out->elements = malloc(sizeof(void*));
	out->elements[0] = current;

	destroy_work_args(wargs);

	return out;
}


static work_args_t * init_work_args(void * arr, uint64_t lo, uint64_t hi, uint64_t * want, uint64_t n_want)
{
	work_args_t * out = calloc(1,sizeof(work_args_t));
	out->hi = hi;
	out->lo = lo;
	out->arr = arr;
	out->n_want = n_want;
	out->want = want;

	out->found_idxs = calloc(out->n_want, sizeof(uint64_t));
	out->found_flags = calloc(out->n_want, sizeof(uint8_t));
	out->done_flag = calloc(1,sizeof(uint8_t));
	return out;
}

static void destroy_work_args(work_args_t * args)
{
	if (args->found_idxs) 	free(args->found_idxs);
	if (args->found_flags) 	free(args->found_flags);
	if (args->done_flag) 	free(args->done_flag);
	if (args->q)			dll_destroy(args->q);
	if (args->out_buff)		dll_destroy(args->out_buff);
	if (args->parents)	free(args->parents);
	if (args->bfs_flags)	free(args->bfs_flags);
	free(args);
}

static void* id_check(void* worker_args)
{
	
	work_args_t * args = (work_args_t *) worker_args;
	if (!args->comp) return NULL;
	size_t i = 0;
	for (i = 0; i < args->n_want && !args->found_flags[i]; i++)
	{
		if (args->comp(args->arr, args->want[i], args->idx))
		{
			args->found_flags[i]++;
			args->found_idxs[i] = args->idx;			
		}
	}
	return NULL;
}

static uint8_t user_comp(void* arr, uint64_t want, uint64_t idx)
{
	user * users = (user *) arr;
	if (users[idx].user_id == want);
	{
		LOG_D("Found the desired ID (%lu) at idx %lu", want, idx);
		return 1;
	}
	return 0;
}

static uint8_t post_comp(void* arr, uint64_t want, uint64_t idx)
{
	post * posts = (post *) arr;
	if (posts[idx].pst_id == want)
	{
		LOG_D("Found the desired ID (%lu) at idx %lu", want, idx);
		return 1;
	}
	return 0;
}


static void * BFS_work(void* worker_args)
{
	work_args_t * args = (work_args_t *) worker_args;

	kin_t * kin = NULL;

	dll_t * q = args->q;
	dll_t * out_buff = args->out_buff;
	uint64_t i = 0;
	uint64_t * first_idx = malloc(sizeof(uint64_t));
	*first_idx = args->idx;
	//Queue its children
	dll_enqueue(q, first_idx);
	//Pop its children
	LOG_D("Done flag: %x", args->done_flag[0]);
	while (q->size && !args->done_flag[0])
	{
		args->idx = *(uint64_t *) dll_pop(q);		
		
		kin = args->work(worker_args);
		dll_enqueue(out_buff, kin->self);
		for (i = 0; i < kin->n_children; i++)
		{
			dll_enqueue(q, &kin->idxs[i]);
			args->idx = kin->idxs[i];
			id_check(args);
		}
		if (kin) free(kin);
		kin = NULL;
		if (args->check_done) args->check_done(args);
	}
	
	args->result->n_elements = out_buff->size;
	args->result->elements = malloc(sizeof(void*)*args->result->n_elements);
	LOG_D("Found %lu reposts!", args->result->n_elements);
	for (i = 0; i < args->result->n_elements && out_buff->size; i++)
	{
		args->result->elements[i] = dll_pop(out_buff);
	}
	free(first_idx);
}


static void * find_reposts_worker(void* worker_args)
{
	work_args_t * args = (work_args_t *) worker_args;
	post * posts = args->arr;

	post * self = &posts[args->idx];

	kin_t * out = calloc(1, sizeof(kin_t));
	out->self = (void*) self;
	out->n_children = self->n_reposted;
	out->idxs = self->reposted_idxs;
	LOG_V("Post %lu being added, with %lu children", args->idx, out->n_children);
	return (void*) out;
}



static void * find_original_worker(void* worker_args)
{
	work_args_t * args = (work_args_t *) worker_args;
	post * posts = args->arr;
	uint64_t i = 0;
	post * self = &posts[args->idx];

	for (i = 0; i < self->n_reposted; i++)
	{
		args->parents[self->reposted_idxs[i]] = args->idx;
		args->bfs_flags[self->reposted_idxs[i]] = 1;
	}
}

static void * done_if_found_one(void* wargs)
{
	work_args_t * args = (work_args_t *) wargs;

	if (args->found_flags[0]) args->done_flag[0]++;
}


