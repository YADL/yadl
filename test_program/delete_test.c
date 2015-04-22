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
extern int delete_file();
int readfilecatalog()
{
	return ((int)mock());
}

static void leak_memory_test(void **state)
{
	will_return(readfilecatalog, 0);
	assert_int_equal(delete_file(), 0);
}


int main(int argc, char* argv[])
{

	const UnitTest tests[] = {
        unit_test(leak_memory_test),
        };
    	return run_tests(tests, "delete_allocate_module_test");
}
