#include "lexer.h"
#include "parser.h"
#include "symbol.h"
#include "term.h"
#include "test.h"
#include <string.h>

TEST(parser_eof_position) {
    token_t *tokens = lex_string("x");
    token_t *cursor = tokens;

    term_t *t = parse(&cursor, NULL);

    TEST_ASSERT(t->type == TM_VAR);
    TEST_ASSERT(t->data.var == sym_intern("x"));

    TEST_ASSERT(t != NULL);
    TEST_ASSERT(cursor == NULL);

    term_destroy(t);
    lex_destroy_tokens(tokens);
    return PASSED;
}

TEST(parser_multiple_app) {
    token_t *tokens = lex_string("f x y z");
    token_t *cursor = tokens;

    term_t *t = parse(&cursor, NULL);

    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_APP);
    TEST_ASSERT(cursor == NULL);

    term_destroy(t);
    lex_destroy_tokens(tokens);
    return PASSED;
}

TEST(parser_multiple_app_parenthesis) {
    token_t *tokens = lex_string("(((((((((f x) y) z)))))))");
    token_t *cursor = tokens;

    term_t *t = parse(&cursor, NULL);

    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_APP);
    TEST_ASSERT(cursor == NULL);

    term_destroy(t);
    lex_destroy_tokens(tokens);
    return PASSED;
}

TEST(parser_identity) {
    term_t *t = term_eval(parse_string("(\\x.x) A"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_VAR);
    TEST_ASSERT(t->data.var == sym_intern("A"));
    term_destroy(t);
    return PASSED;
}

TEST(parser_double_app) {
    term_t *t = term_eval(parse_string("(\\x.\\y. x) A B"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_VAR);
    TEST_ASSERT(t->data.var == sym_intern("A"));
    term_destroy(t);
    return PASSED;
}

TEST(parser_nested_reduction) {
    term_t *t = term_eval(parse_string("(\\f. f A) (\\x. x)"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_VAR);
    TEST_ASSERT(t->data.var == sym_intern("A"));
    term_destroy(t);
    return PASSED;
}

TEST(parser_shadowing) {
    term_t *t = term_eval(parse_string("(\\x.\\x.x) A B"));
    TEST_ASSERT(t != NULL);
    TEST_ASSERT(t->type == TM_VAR);
    TEST_ASSERT(t->data.var == sym_intern("B"));
    term_destroy(t);
    return PASSED;
}
