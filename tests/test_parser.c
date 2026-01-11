#include "lexer.h"
#include "parser.h"
#include "symbol.h"
#include "term.h"
#include "test.h"
#include <string.h>

TEST(parser_eof_position) {
    token_t *tokens = lex_string("\\x.x");
    token_t *cursor = tokens;

    term_t *t = parse(&cursor, NULL);

    TEST_ASSERT(t->type == TM_ABS);
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(cursor == NULL);

    term_destroy(t);
    lex_destroy_tokens(tokens);
    return PASSED;
}

TEST(parser_multiple_app) {
    term_t *t = parse_string("f x y z");

    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_APP);
    TEST_ASSERT(t->data.app.right->type == TM_VAR);
    TEST_ASSERT(t->data.app.right->data.var.sym == sym_intern("z"));

    term_destroy(t);
    return PASSED;
}

TEST(parser_identity) {
    term_t *t = term_eval(parse_string("(\\x.x) A"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_VAR);
    TEST_ASSERT(t->data.var.sym == sym_intern("A"));
    term_destroy(t);
    return PASSED;
}

TEST(parser_double_app) {
    term_t *t = term_eval(parse_string("(\\x.\\y. x) A B"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_VAR);
    TEST_ASSERT(t->data.var.sym == sym_intern("A"));
    term_destroy(t);
    return PASSED;
}

TEST(parser_nested_reduction) {
    term_t *t = term_eval(parse_string("(\\f. f A) (\\x. x)"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_VAR);
    TEST_ASSERT(t->data.var.sym == sym_intern("A"));
    term_destroy(t);
    return PASSED;
}

TEST(parser_shadowing) {
    term_t *t = term_eval(parse_string("(\\x.\\x.x) A B"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_VAR);
    TEST_ASSERT(t->data.var.sym == sym_intern("B"));
    term_destroy(t);
    return PASSED;
}

TEST(parser_deep_beta_reduction) {
    term_t *t = term_fuck(parse_string("(\\f.\\x.(f x)) (\\y.y)"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_ABS);
    TEST_ASSERT(t->data.abs.body->type == TM_VAR);
    TEST_ASSERT(t->data.abs.body->data.var.index == 0);
    term_destroy(t);
    return PASSED;
}

TEST(parser_church_number_add) {
    term_t *t =
        term_eval(parse_string("+ (\\f.\\x.(f (f x))) (\\f.\\x.(f x))"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_ABS);
    TEST_ASSERT(term_as_church(t) == 3);
    term_destroy(t);
    return PASSED;
}

TEST(parser_church_number_mul) {
    term_t *t = term_eval(
        parse_string("* (\\f.\\x.(f (f (f x)))) (\\f.\\x.(f (f x)))"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_ABS);
    TEST_ASSERT(term_as_church(t) == 6);
    term_destroy(t);
    return PASSED;
}

TEST(parser_int) {
    term_t *t = parse_string("14");
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(term_as_church(t) == 14);
    term_destroy(t);
    return PASSED;
}
