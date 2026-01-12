#include "parser.h"
#include "term.h"
#include "test.h"

TEST(simplified_exp_fact) {
    term_t *t = term_eval(parse_string(
        "(\\Y g n. Y g n) (\\f. (\\x. f (x x)) (\\x. f (x x))) (\\f n. (\\p x "
        "y. p x y) ((\\n. n (\\x. (\\x y. y)) (\\x y. x)) n) (\\f x. f x) "
        "((\\m n f x. m (n f) x) n (f ((\\n f x. n (\\g h. h (g f)) (\\u. x) "
        "(\\u. u)) n)))) 3"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(term_as_church(t) == 3 * 2 * 1);
    term_destroy(t);
    return PASSED;
}

TEST(simplified_exp_SKKI) {
    term_t *t = term_eval(
        parse_string("(\\x y z. x z (y z)) (\\a b. a) (\\c d. c) \\e. e"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_ABS);
    TEST_ASSERT(t->data.abs.body->type == TM_VAR);
    TEST_ASSERT(t->data.abs.body->data.var.index == 0);
    term_destroy(t);
    return PASSED;
}
