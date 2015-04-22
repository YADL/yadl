#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <inttypes.h>
#include <cmockery/cmockery.h>
extern int
init_catalog_store();
extern int
readfilecatalog();
extern int
writecatalog(char* filename);
int
fini_catalog_store();

// Test case that fails as leak_memory() leaks a dynamically allocated block.
static void leak_memory_test(void **state)
{
	readfilecatalog();
}
// Test case that fails as buffer_overflow() corrupts an allocated block.
static void buffer_overflow_test(void **state)
{
    	readfilecatalog();
}
// Test case that fails as buffer_underflow() corrupts an allocated block.
static void buffer_underflow_test(void **state)
{
   	readfilecatalog();
}
static void test_writecatalog(void **state)
{
	expect_string(writecatalog,filename, "/root/test_program/new.txt");
	assert_int_equal(writecatalog("/root/test_program/new.txt"), 0);

}
static void test_return_values(void **state)
{

	assert_int_equal(fini_catalog_store(), 0);
	assert_int_equal(init_catalog_store(), 0);
}





int main(int argc, char* argv[])
{

	init_catalog_store();
	const UnitTest tests[] = {
    unit_test(leak_memory_test),
    unit_test(buffer_overflow_test),
    unit_test(buffer_underflow_test),
	unit_test(test_writecatalog),
	unit_test(test_return_values),
    };
    return run_tests(tests, "catalog_allocate_module_test");
}
