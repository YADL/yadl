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
extern int 
comparepath(char out[]);

/* Test case that fails as leak_memory() leaks a dynamically allocated block.*/
static void leak_memory_test(void **state) 
{
        
        readfilecatalog();
        comparepath("/root/test_program/new.txt");

}

/* Test case that fails as buffer_overflow() corrupts an allocated block.*/
static void buffer_overflow_test(void **state) 
{
        
        readfilecatalog();
        comparepath("/root/test_program/new.txt");

}

/* Test case that fails as buffer_underflow() corrupts an allocated block.*/
static void buffer_underflow_test(void **state) 
{
        
        readfilecatalog();
        comparepath("/root/test_program/new.txt");

}

/* Test case to test return value of writecatalog*/
static void test_writecatalog(void **state) 
{ 
        
        assert_int_equal(writecatalog("/root/test_program/new.txt"), 0);
        
}

/* Test case to test return value of fini and initcatalogstore*/
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
