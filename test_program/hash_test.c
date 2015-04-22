#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <inttypes.h>
#include <cmockery/cmockery.h>
extern int
init_hash_store();
extern int
insert_hash(char *buff,int offset);
int
searchhash(char *out);
extern int
getposition(char* hash);
extern int
fini_hash_store();
// Test case that fails as leak_memory() leaks a dynamically allocated block.
static void leak_memory_test(void **state)
{
	searchhash("6e6078646c99008a92af2b9c4489860133cfd3a6");
	getposition("6e6078646c99008a92af2b9c4489860133cfd3a6");
}
// Test case that fails as buffer_overflow() corrupts an allocated block.
static void buffer_overflow_test(void **state)
{
    searchhash("6e6078646c99008a92af2b9c4489860133cfd3a6");
    getposition("6e6078646c99008a92af2b9c4489860133cfd3a6");
}
// Test case that fails as buffer_underflow() corrupts an allocated block.
static void buffer_underflow_test(void **state)
{
   	searchhash("6e6078646c99008a92af2b9c4489860133cfd3a6");
	getposition("6e6078646c99008a92af2b9c4489860133cfd3a6");
}
static void test_writehash(void **state)
{
	insert_hash("6e6078646c99008a92af2b9c4489860133cfd3a6",57);

}
static void test_return_values(void **state)
{

	assert_int_equal(init_hash_store(), 0);
	assert_int_equal(searchhash("6e6078646c99008a92af2b9c4489860133cfd3a6"), 0);
	assert_int_equal(insert_hash("6e6078646c99008a92af2b9c4482360133cfd335",67),0);
	assert_int_equal(fini_hash_store(), 0);


}

int main(int argc, char* argv[])
{

	init_hash_store();
	const UnitTest tests[] = {
    unit_test(leak_memory_test),
    unit_test(buffer_overflow_test),
    unit_test(buffer_underflow_test),
	unit_test(test_writehash),
	unit_test(test_return_values),
    };
    return run_tests(tests, "hash_allocate_module_test");
}
