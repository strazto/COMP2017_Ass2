#include "mine.h"

uint8_t find_orig_manip_sequential(work_args_t * args, kin_t * elem);
uint8_t find_orig_manip_adj(work_args_t * args, kin_t * elem);
uint8_t find_orig_manip(work_args_t * args, kin_t * elem);


/**
 * For now, will leave this, but could consider for when Mthreading, multiple BFSs, 
 * initially for the index u want, but then maybe also
 * 
 */

result * find_original_wrapper(post* posts, size_t count, uint64_t post_id, query_helper* q_h)
{
	uint64_t * want = malloc(sizeof(uint64_t));
	*want = post_id;
	uint64_t i = 0;
	work_args_t * args = init_work_args(posts, 0, count, want, 1);
	result * out = calloc(1,sizeof(result));
	args->get_elem = post_getter;
	args->manipulate_elem = find_orig_manip_sequential;

	work_on_segment(args);

	if (!args->found_flags[0])
	{
		destroy_work_args(args);
		return out;
	}

	out->n_elements = 1;
	out->elements = malloc(sizeof(void*));

	i = args->found_idxs[0];
	while (args->bfs_flags[i])
	{
		i = args->pred[i];
	}

	out->elements[0] = &posts[i];

	return out;
}


uint8_t find_orig_manip_sequential(work_args_t * args, kin_t * elem)
{
	size_t i = 0;
	//Consider ditching this line ////////////////
	// if (args->bfs_flags[elem->idx]) return 0;
	for (i = 0; i < elem->n_out ; i++)
	{
		args->pred[elem->idxs_out[i]] = elem->idx;
		args->bfs_flags[elem->idxs_out[i]] = 1;
	}
	if (elem->id == args->want[0])
	{
		args->found_idxs[0] = elem->idx;
		args->found_flags[0] = 1;
	}
	return 1;
}
