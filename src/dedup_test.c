#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <inttypes.h>
#include <cmockery/cmockery.h>

// A test case that does nothing and succeeds.
static void null_test_success(void **state) {
    (void) state;
}

int main(void) {
    const UnitTest tests[] = {
        unit_test(null_test_success),
    };

    return run_tests(tests, "dedup_test");
}
