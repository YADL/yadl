#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <inttypes.h>
#include <cmockery/cmockery.h>
extern int get_block(int pos);
extern int init_block_store();
extern int insert_block(char *buffer,size_t length);
extern int fini_block_store();
static void test_return_values(void **state)
{
	expect_value(get_block, pos, 5);
	assert_int_equal(init_block_store(), 0);
	assert_int_equal(insert_block("increase in level",17), 0);
	assert_int_equal(get_block(5), 0);
}


// Test case that fails as leak_memory() leaks a dynamically allocated block.
static void leak_memory_test(void **state)
{
	expect_value(get_block, pos, 5);
	get_block(5);
}
// Test case that fails as buffer_overflow() corrupts an allocated block.
static void buffer_overflow_test(void **state)
{
	expect_value(get_block, pos, 5);
	get_block(5);
}
// Test case that fails as buffer_underflow() corrupts an allocated block.
static void buffer_underflow_test(void **state)
{
   	expect_value(get_block, pos, 5);
	get_block(5);
}
/* Funtion to test return value of fini_block_store
*/
static void test_fini_block_store(void **state)
{
	assert_int_equal(fini_block_store(), 0);

}


int main(int argc, char* argv[])
{


	const UnitTest tests[] = {
	unit_test(test_return_values),
	unit_test(leak_memory_test),
    unit_test(buffer_overflow_test),
    unit_test(buffer_underflow_test),
	unit_test(test_fini_block_store),
    };
    return run_tests(tests, "block_allocate_module_test");
}
