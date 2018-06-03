#ifndef CONSTS_H
#define CONSTS_H
#include "supergraph.h"


//Recursive query operations
//All should have void* f(void*) signatures




result * find_all_reposts_wrapper(post* posts, size_t count, uint64_t post_id, query_helper* helper);



/**
 * Exposed wrapper for find_original
 * @param posts: Array of post members to search through
 * @param count: Number of posts within array
 * @param post_id: the id of the post to be returned in result
 * @param query_helper: The query helper object
 * @return Result, with pointer to original element, and size 1, or null elements and size 0 if id not found.
 */
result * find_original_wrapper(post* posts, size_t count, uint64_t post_id, query_helper* helper);


#endif
