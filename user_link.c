#include "mine.h"

uint8_t find_user_link_manip_sequential(work_args_t * args, kin_t * elem);
uint8_t find_user_link_manip_adj(work_args_t * args, kin_t * elem);
uint8_t find_user_link_manip(work_args_t * args, kin_t * elem);

result* shortest_user_link_wrapper(user* users, size_t count, uint64_t userA, uint64_t userB, query_helper* helper)
{
	uint64_t n_wanted = 2;
	uint64_t * want = malloc(sizeof(uint64_t)*n_wanted);
	want[0] = userA;
	want[1] = userB;
	dll_t * out_buff;
	uint64_t i = 0;
	work_args_t * args = init_work_args(users, 0, count, want, n_wanted);
	result * out = calloc(1,sizeof(result));
	args->get_elem = user_getter;
	args->manipulate_elem = find_user_link_manip_sequential;
	args->manipulate_adjacent = find_user_link_manip_adj;

	work_on_segment(args);

	if (!args->done_flag[0])
	{
		destroy_work_args(args);
		LOG_I("Didn't find A (%lu) and B (%lu)", args->want[0], args->want[1]);
		return out;
	}
	args->done_flag[0] = 0;
	args->manipulate_elem = find_user_link_manip;
	args->idx = args->found_idxs[0];

	BFS_work(args);

	out_buff = args->qs[1];
	
	for (i = args->pred[args->found_idxs[1]]; args->bfs_flags[i] && i != args->found_idxs[0]; i = args->pred[i])
	{
		//Actually the loop guard did it all i think. for fun just print it
		LOG_I("Queueing %lu", i);
		dll_push(out_buff, &users[i]);
	}

	out->n_elements = out_buff->size;
	if (out->n_elements) out->elements = malloc(sizeof(void*)*out->n_elements);
	
	for (i = 0; out_buff->size; i++)
	{
		out->elements[i] = dll_pop(out_buff);
	}
	destroy_work_args(args);
	return out;
}

uint8_t find_user_link_manip_sequential(work_args_t * args, kin_t * elem)
{
	if (elem->id == args->want[0])
	{
		args->found_idxs[0] = elem->idx;
		args->found_flags[0] = 1;

		LOG_I("Found user A @ %lu", elem->idx);
	}
	if (elem->id == args->want[1])
	{
		args->found_idxs[1] = elem->idx;
		args->found_flags[1] = 1;
		LOG_I("Found user B @ %lu", elem->idx);
	}
	if (args->found_flags[0] && args->found_flags[1])
	{
		*args->done_flag = 1;
		return 1;
	}

	return 0;
}

uint8_t find_user_link_manip(work_args_t * args, kin_t * elem)
{
	//Do nothin!
	return 0;
}

uint8_t find_user_link_manip_adj(work_args_t * args, kin_t * elem)
{
	if (elem->id == args->want[1])
	{
		args->done_flag[0] = 1;
	}
	return generic_manip_adj(args, elem);
}