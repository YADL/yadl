#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include <stdarg.h>
#include<sys/types.h>
#include <stddef.h>
#include <setjmp.h>
#include <inttypes.h>
#include <cmockery/cmockery.h>
#include<cmockery/pbc.h>
#include<fcntl.h>
int	fd_stub;
int write_to_stub(char buff[],size_t length,int fd_stub,int b_offset,int e_offset);
int searchstubhash(int fd_stub,int b_offset,int e_offset);
// Test case that fails as leak_memory() leaks a dynamically allocated block.
static void leak_memory_test(void **state)
{
	fd_stub=open("Dedup_new.txt",O_APPEND|O_CREAT|O_RDWR);
	expect_value(searchstubhash, b_offset, 1000);
	expect_value(searchstubhash, e_offset, 1999);
	searchstubhash(fd_stub, 1000,1999);
	close(fd_stub);
}
// Test case that fails as buffer_overflow() corrupts an allocated block.
static void buffer_overflow_test(void **state)
{
	fd_stub=open("Dedup_new.txt",O_APPEND|O_CREAT|O_RDWR);
	expect_value(searchstubhash, b_offset, 1000);
	expect_value(searchstubhash, e_offset, 1999);
    searchstubhash(fd_stub, 1000,1999);
	close(fd_stub);
}
// Test case that fails as buffer_underflow() corrupts an allocated block.
static void buffer_underflow_test(void **state)
{
   	fd_stub=open("Dedup_new.txt",O_APPEND|O_CREAT|O_RDWR);
	expect_value(searchstubhash, b_offset, 1000);
	expect_value(searchstubhash, e_offset, 1999);
	searchstubhash(fd_stub, 1000,1999);
	close(fd_stub);
}
static void test_searchstubhash(void **state)
{
	fd_stub=open("Dedup_new.txt",O_APPEND|O_CREAT|O_RDWR);
	expect_value(searchstubhash, b_offset, 1000);
	expect_value(searchstubhash, e_offset, 1999);
	assert_int_equal(searchstubhash(fd_stub, 1000,1999),0);
	close(fd_stub);
}
static void test_write_to_stub(void **state)
{

	fd_stub=open("Dedup_new.txt",O_APPEND|O_CREAT|O_RDWR);
    assert_int_equal(write_to_stub("b8c1b4fe198a7cd551c6baa9ef31136627d027b2",1000,fd_stub,1000,1999),0);
	close(fd_stub);
}
int main(int argc, char* argv[]) {
    const UnitTest tests[] = {
        unit_test(test_write_to_stub),
        unit_test(test_searchstubhash),
		unit_test(leak_memory_test),
		unit_test(buffer_overflow_test),
		unit_test(buffer_underflow_test),
          };
    return run_tests(tests, "stub");
}
