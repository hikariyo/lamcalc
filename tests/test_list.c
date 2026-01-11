#include "parser.h"
#include "term.h"
#include "test.h"

/*
 Tests for list builtins: nil, isnil, cons, head, tail.
 Also tests higher-order recursive helpers (length, sum, product)
 written with Y-combinator and builtins.
*/

TEST(list_head_tail_cons_nil) {
    // head of (cons 1 (cons 2 nil)) -> 1
    term_t *t1 = term_eval(parse_string("head (cons 1 (cons 2 nil))"));
    TEST_ASSERT(t1 != NULL);
    TEST_ASSERT(term_as_church(t1) == 1);
    term_destroy(t1);

    // head (tail (cons 1 (cons 2 (cons 3 nil)))) -> 2
    term_t *t2 =
        term_eval(parse_string("head (tail (cons 1 (cons 2 (cons 3 nil))))"));
    TEST_ASSERT(t2 != NULL);
    TEST_ASSERT(term_as_church(t2) == 2);
    term_destroy(t2);

    // tail of single-element list -> nil (isnil should return true)
    term_t *isnil_res =
        term_eval(parse_string("((isnil (tail (cons 1 nil))) 1 0)"));
    TEST_ASSERT(isnil_res != NULL);
    TEST_ASSERT(term_as_church(isnil_res) == 1);
    term_destroy(isnil_res);

    return PASSED;
}

TEST(list_isnil_behaviour) {
    // isnil nil -> true (we convert boolean to number via applying to 1 and 0)
    term_t *t1 = term_eval(parse_string("((isnil nil) 1 0)"));
    TEST_ASSERT(t1 != NULL);
    TEST_ASSERT(term_as_church(t1) == 1);
    term_destroy(t1);

    // isnil (cons 1 nil) -> false
    term_t *t2 = term_eval(parse_string("((isnil (cons 1 nil)) 1 0)"));
    TEST_ASSERT(t2 != NULL);
    TEST_ASSERT(term_as_church(t2) == 0);
    term_destroy(t2);

    return PASSED;
}

TEST(list_length_sum_product) {
    // length = Y (\f.\l. ((isnil l) 0 (+ 1 (f (tail l))))) LIST
    const char *list123 = "(cons 1 (cons 2 (cons 3 nil)))";
    char buf_len[512];
    snprintf(buf_len, sizeof(buf_len),
             "Y (\\f.\\l. ((isnil l) 0 (+ 1 (f (tail l))))) %s", list123);
    term_t *len = term_eval(parse_string(buf_len));
    TEST_ASSERT(len != NULL);
    TEST_ASSERT(term_as_church(len) == 3);
    term_destroy(len);

    // sum = Y (\f.\l. ((isnil l) 0 (+ (head l) (f (tail l))))) LIST
    char buf_sum[512];
    snprintf(buf_sum, sizeof(buf_sum),
             "Y (\\f.\\l. ((isnil l) 0 (+ (head l) (f (tail l))))) %s",
             list123);
    term_t *sum = term_eval(parse_string(buf_sum));
    TEST_ASSERT(sum != NULL);
    TEST_ASSERT(term_as_church(sum) == 6); // 1+2+3
    term_destroy(sum);

    // product = Y (\f.\l. ((isnil l) 1 (* (head l) (f (tail l))))) LIST2
    const char *list234 = "(cons 2 (cons 3 (cons 4 nil)))";
    char buf_prod[512];
    snprintf(buf_prod, sizeof(buf_prod),
             "Y (\\f.\\l. ((isnil l) 1 (* (head l) (f (tail l))))) %s",
             list234);
    term_t *prod = term_eval(parse_string(buf_prod));
    TEST_ASSERT(prod != NULL);
    TEST_ASSERT(term_as_church(prod) == 24); // 2*3*4
    term_destroy(prod);

    // length of nil == 0
    term_t *len_nil = term_eval(
        parse_string("Y (\\f.\\l. ((isnil l) 0 (+ 1 (f (tail l))))) nil"));
    TEST_ASSERT(len_nil != NULL);
    TEST_ASSERT(term_as_church(len_nil) == 0);
    term_destroy(len_nil);

    // sum of nil == 0
    term_t *sum_nil = term_eval(parse_string(
        "Y (\\f.\\l. ((isnil l) 0 (+ (head l) (f (tail l))))) nil"));
    TEST_ASSERT(sum_nil != NULL);
    TEST_ASSERT(term_as_church(sum_nil) == 0);
    term_destroy(sum_nil);

    // product of nil == 1 (empty product)
    term_t *prod_nil = term_eval(parse_string(
        "Y (\\f.\\l. ((isnil l) 1 (* (head l) (f (tail l))))) nil"));
    TEST_ASSERT(prod_nil != NULL);
    TEST_ASSERT(term_as_church(prod_nil) == 1);
    term_destroy(prod_nil);

    return PASSED;
}
