#include "DLL_unsafe.h"
#include "thread_test.h"
#include "log.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>


static int init_example(void** state);
static dll_t * rm_7_seq(dll_t * list);
static int destroy_example(void** state);

static void test_remove_7(void ** state);
static int teardown_list(void ** state);
void main(void)
{
	const struct CMUnitTest rm_7[] = 
	{
		cmocka_unit_test(test_remove_7),
		cmocka_unit_test(test_remove_7),
		cmocka_unit_test(test_remove_7),
		cmocka_unit_test(test_remove_7),
		cmocka_unit_test(test_remove_7),
		cmocka_unit_test(test_remove_7),
		cmocka_unit_test(test_remove_7),
		cmocka_unit_test(test_remove_7)	
	};
	cmocka_run_group_tests_name("REMOVE_7", rm_7, init_example, teardown_list);

}


static int init_example(void** state)
{
	dll_t * list = dll_init();
	int n_elements = 10;
	int64_t n_removed = 0;
	
	int to_push = 0;
	int64_t count = 0;	

	if (!list)
	{
		LOG_E("No list %c", '!');
	}
	for (int i = 0; i < n_elements; i++)
	{
		enqueue(list, (void*) i);
	}

	for (int i = 0; i < 50; i++)
	{
		to_push = i;
		if (! (i %3)) to_push = 7;

		push(list, (void*) ( to_push));
	}


	LOG_I("List size: %lu",list->size);

	node_t * current = first(list);

	while (current && current != list->tail)
	{
		LOG_V("Element: %li", (int64_t) current->val);
		if (7 == (int64_t) current->val) count++;
		current = current->prev;
	}
	LOG_I("N 7s %li", count);
	count = 0;

	*state = list;
	return 0;
}




static dll_t * rm_7_seq(dll_t * list)
{
	
	dll_t * out = dll_init();
	node_t * current = NULL;

	for (current = first(list); current && current != list->tail ; current = current->prev)
	{
		LOG_V("Element: %li", (int64_t) current->val);
		if (7 != (int64_t) current->val) enqueue(out, current->val);
	}
	return out;
}


static void test_remove_7(void ** state)
{
	dll_t * list = (dll_t*) *state;
	dll_t * expected = rm_7_seq(list);
	int64_t count = 0;
	int n_threads = 8;
	void * retval;
	pthread_t * threads = malloc(sizeof(pthread_t)*n_threads);
	node_t * current = NULL;
	node_t * expect = NULL;
	int64_dll_args_t args = 
	{
		.list = list,
		.element = 7
	};

	for (int i = 0; i < n_threads; i++)
	{
		LOG_I("Initializing thread %i", i);
		pthread_create(&threads[i], NULL, remove_number, &args);
	}

	for (int i = 0; i < n_threads; i++)
	{
		pthread_join(threads[i], &retval);
		LOG_I("Thread %i, n removed %li", i,(int64_t) retval);
		count += (int64_t) retval;

	}
	LOG_I("N removed total: %li", count);



	LOG_I("List size: %lu",list->size);
	assert_int_equal(expected->size, list->size);


	current = first(list);

	for (expect = first(expected); expect && expect != expected->tail && current && current != list->tail; expect = expect->prev)
	{
		LOG_V("EXP %li, ACT %li", (int64_t) expect->val, (int64_t) current->val);
		assert_ptr_equal(expect->val,current->val);
		current = current->prev;
	}
	free(expected);

}

static int teardown_list(void ** state)
{
	dll_t * to_rm = (dll_t*) *state;
	dll_destroy(to_rm);
	*state = NULL;
	return 0;
}