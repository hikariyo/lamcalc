#include "test.h"
#include "symbol.h"
#include <stdio.h>

int test_passed_count;
int test_failed_count;

int main() {
    printf(COLOR_CYAN "[==========] %d test(s) ran.\n" COLOR_RESET,
           test_passed_count + test_failed_count);

    if (test_passed_count > 0) {
        printf(COLOR_GREEN "[  PASSED  ] %d test(s).\n" COLOR_RESET,
               test_passed_count);
    }

    if (test_failed_count > 0) {
        printf(COLOR_RED "[  FAILED  ] %d test(s).\n" COLOR_RESET,
               test_failed_count);
    }

    sym_destory();

    return 0;
}
