#include "parser.h"
#include "term.h"
#include "test.h"

#include "parser.h"
#include "term.h"
#include "test.h"
#include <stdio.h>

TEST(builtin_add) {
    term_t *t = term_eval(parse_string("+ 4 6"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(term_as_church(t) == 10);
    term_destroy(t);
    return PASSED;
}

TEST(builtin_mul) {
    term_t *t = term_eval(parse_string("* 3 7"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(term_as_church(t) == 21);
    term_destroy(t);
    return PASSED;
}

TEST(builtin_pred) {
    // pred 5 -> 4
    term_t *t1 = term_eval(parse_string("pred 5"));
    TEST_ASSERT(term_as_church(t1) == 4);
    term_destroy(t1);

    // pred 0 -> 0
    term_t *t2 = term_eval(parse_string("pred 0"));
    TEST_ASSERT(term_as_church(t2) == 0);
    term_destroy(t2);
    return PASSED;
}

TEST(builtin_ifz) {
    // ifz 0 true false -> true (1)
    term_t *t1 = term_eval(parse_string("ifz 0 1 2"));
    TEST_ASSERT(term_as_church(t1) == 1);
    term_destroy(t1);

    // ifz 5 true false -> false (2)
    term_t *t2 = term_eval(parse_string("ifz 5 1 2"));
    TEST_ASSERT(term_as_church(t2) == 2);
    term_destroy(t2);
    return PASSED;
}

TEST(builtin_sub_mock) {
    term_t *t = term_eval(parse_string("(\\n.\\m. m pred n) 10 4"));
    TEST_ASSERT(term_as_church(t) == 6);
    term_destroy(t);
    return PASSED;
}

TEST(builtin_recursive_sum) {
    term_t *t =
        term_eval(parse_string("Y (\\f.\\n. ifz n 0 (+ n (f (pred n)))) 5"));

    // sum(5) = 15
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(term_as_church(t) == 15);
    term_destroy(t);
    return PASSED;
}

TEST(builtin_recursive_factorial) {
    const char *fact_str = "Y (\\f.\\n. ifz n 1 (* n (f (pred n)))) 4";
    term_t *t = term_eval(parse_string(fact_str));

    TEST_ASSERT(t != NULL);
    TEST_ASSERT(term_as_church(t) == 24);
    term_destroy(t);
    return PASSED;
}

TEST(builtin_stress_large_church) {
    term_t *t = term_eval(parse_string("+ 50 50"));
    TEST_ASSERT(term_as_church(t) == 100);
    term_destroy(t);
    return PASSED;
}
