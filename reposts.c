#include "mine.h"
uint8_t find_all_reposts_manip_sequential(work_args_t * args, kin_t * elem);
uint8_t find_all_reposts_manip_adj(work_args_t * args, kin_t * elem);
uint8_t find_all_reposts_manip(work_args_t * args, kin_t * elem);
//uint8_t find_all_reposts_worker(work_args_t * args, kin_t * elem);

result* find_all_reposts_wrapper(post* posts, size_t count, uint64_t post_id, query_helper* helper) 
{
	result * out = calloc(1, sizeof(result));
	uint64_t * want = malloc(sizeof(uint64_t));
	*want = post_id;
	int i = 0;
	work_args_t * args = init_work_args(posts, 0, count, want, 1);
	dll_t * out_queue = args->qs[1];

	args->get_elem = post_getter;
	args->manipulate_elem = find_all_reposts_manip_sequential;
	args->manipulate_adjacent = find_all_reposts_manip_adj;

	work_on_segment(args);
	args->done_flag[0] = 0;

	if (!args->found_flags[0])
	{
		destroy_work_args(args);
		return out;
	}

	args->manipulate_elem = find_all_reposts_manip;
	args->idx = args->found_idxs[0];

	LOG_I("value of args->idx = %lu", args->idx);

	BFS_work(args);

	out->n_elements = out_queue->size;
	if (out->n_elements) out->elements = malloc(sizeof(void*)*out->n_elements);
	for (i = 0; out_queue->size; i++)
	{
		out->elements[i] = dll_pop(out_queue);
	}
	destroy_work_args(args);
	return out;
}


uint8_t find_all_reposts_manip(work_args_t * args, kin_t * elem)
{
	LOG_V("Going to queue elem: %lu", elem->idx);
	dll_t * q = args->qs[1];
	dll_enqueue(q,elem->self);
	return 0;
}

uint8_t find_all_reposts_manip_adj(work_args_t * args, kin_t * elem)
{
	return generic_manip_adj(args, elem);
}

uint8_t find_all_reposts_manip_sequential(work_args_t * args, kin_t * elem)
{
	if (elem->id != *args->want) return 0;
	args->found_idxs[0] = elem->idx;
	args->found_flags[0] = 1;
	args->done_flag[0] = 1;
	LOG_D("Found elem @ %lu", args->found_idxs[0]);	

	return 0;
}