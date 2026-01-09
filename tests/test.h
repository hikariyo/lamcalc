#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>

#define COLOR_GREEN "\033[32m"
#define COLOR_RED "\033[31m"
#define COLOR_CYAN "\033[36m"
#define COLOR_RESET "\033[0m"
#define COLOR_BOLD "\033[1m"

#define FAILED 0
#define PASSED 1

extern int test_passed_count;
extern int test_failed_count;

#define TEST(name)                                                             \
    static int __Test_##name();                                                \
    __attribute__((constructor)) static void __Register_##name(void) {         \
        printf(COLOR_CYAN "[ RUN      ] %s\n" COLOR_RESET, #name);             \
        if (__Test_##name() == FAILED) {                                       \
            printf(COLOR_RED "[  FAILED  ] %s\n" COLOR_RESET, #name);          \
            test_failed_count++;                                               \
        } else {                                                               \
            printf(COLOR_GREEN "[       OK ] %s\n" COLOR_RESET, #name);        \
            test_passed_count++;                                               \
        }                                                                      \
    }                                                                          \
    static int __Test_##name()

#endif // TEST_H