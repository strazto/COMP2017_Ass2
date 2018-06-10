#include "mine.h"

/*
 * Find the location of an id from within an array
 * :: void* work_args :: 
 * Returns id
 */
 void* work_on_segment(void* work_args_in)
{
	work_args_t * args = (work_args_t *) work_args_in;
	uint64_t i = 0;
	kin_t * elem = NULL;
	for (i = args->lo; i < args->hi && !args->done_flag[0]; i++)
	{
		//Get your element
		elem = args->get_elem(args->arr, i);
		LOG_V("Working on arr[%lu], with %lu children", i, elem->n_out);
		//Now do some work with it
		args->manipulate_elem(args, elem);
		
		free(elem);
		elem = NULL;
	}
	LOG_I("Leaving now, found_idxs[0] = %lu", args->found_idxs[0]);
	return NULL;
}

 work_args_t * init_work_args(void * arr, uint64_t lo, uint64_t hi, uint64_t * want, uint64_t n_want)
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
	out->bfs_flags = calloc(hi -lo, sizeof(uint8_t));
	out->pred = calloc(hi - lo, sizeof(uint64_t));
	out->n_qs = 2;
	out->qs = calloc(out->n_qs,sizeof(dll_t*));
	out->qs[0] = dll_init();
	out->qs[1] = dll_init();

	return out;
}

 void destroy_work_args(work_args_t * args)
{
	if (args->found_idxs) 	free(args->found_idxs);
	if (args->found_flags) 	free(args->found_flags);
	if (args->done_flag) 	free(args->done_flag);	
	if (args->bfs_flags)	free(args->bfs_flags);
	if (args->want)			free(args->want);
	if (args->pred)			free(args->pred);
	if (args->qs)
	{
		for (uint8_t i = 0; i < args->n_qs ; i++)
		{
			dll_destroy(args->qs[i]);
		}
		free(args->qs);
		args->qs = NULL;
	}

	free(args);
}


 void * BFS_work(void* worker_args)
{
	work_args_t * args = (work_args_t *) worker_args;
	kin_t * elem = NULL;
	kin_t * next_elem = NULL;
	uint64_t * index = NULL;
	

	uint64_t i = 0;
	dll_t * q = args->qs[0];

	//Queue its children
	dll_enqueue(q, &args->idx);
	//Pop its children
	LOG_D("Done flag: %x, q->size: %lu", args->done_flag[0], q->size);
	while (q->size && !args->done_flag[0])
	{
		index = (uint64_t*) dll_pop(q);
		elem = args->get_elem(args->arr, *index);
		
		args->manipulate_elem(args, elem);
		LOG_V("Working on arr[%lu], n_out = %lu, n_in %lu", elem->idx, elem->n_out, elem->n_in);
		//For all of the degrees in:
		for (i = 0; i < elem->n_in ; i++)
		{
			next_elem = args->get_elem(args->arr, elem->idxs_in[i]);
			if (args->manipulate_adjacent(args, next_elem))
			{
				args->pred[next_elem->idx] = elem->idx;
				dll_enqueue(q, &elem->idxs_in[i]);
			}

			free(next_elem);
			next_elem = NULL;
		}
		
		for (i = 0; i < elem->n_out ; i++)
		{
			next_elem = args->get_elem(args->arr, elem->idxs_out[i]);
			if (args->manipulate_adjacent(args, next_elem))
			{
				args->pred[next_elem->idx] = elem->idx;
				dll_enqueue(q, &elem->idxs_out[i]);
			}

			free(next_elem);
			next_elem = NULL;
		}
		free(elem);
		elem = NULL;
	}

	return (args->result);
}

void * done_if_found_one(void* wargs)
{
	work_args_t * args = (work_args_t *) wargs;

	if (args->found_flags[0]) args->done_flag[0]++;

	return NULL;
}

kin_t * post_getter(void* arr, uint64_t index)
{
	kin_t * out = calloc(1,sizeof(kin_t));
	post * posts = (post *) arr;
	post * my_post = &posts[index];

	out->self = (void*) my_post;

	out->id = my_post->pst_id;
	out->n_out = my_post->n_reposted;
	out->idxs_out = my_post->reposted_idxs;
	out->idx = index;
	return out;
}

kin_t * user_getter(void* arr, uint64_t index)
{
	kin_t * out = calloc(1, sizeof(kin_t));
	user * my_user = (user *) arr;

	my_user = &my_user[index];
	out->self = (void*) my_user;

	out->id = my_user->user_id;
	out->n_out = my_user->n_following;
	out->idxs_out = my_user->following_idxs;

	out->n_in = my_user->n_followers;
	out->idxs_in = my_user->following_idxs;
	out->idx = index;
	return out;
}

uint8_t generic_manip_adj(work_args_t * args, kin_t * elem)
{
	if (args->bfs_flags[elem->idx]) return 0;
	args->bfs_flags[elem->idx] = 1;
	return 1;
}