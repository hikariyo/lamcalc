#include "lexer.h"
#include "parser.h"
#include "symbol.h"
#include "term.h"
#include "test.h"

TEST(parser_eof_position) {
    token_t *tokens = lex_string("x");
    token_t *cursor = tokens;

    term_t *t = parse(&cursor, NULL);

    TEST_ASSERT(t->type == TM_VAR);
    TEST_ASSERT(t->data.var == sym_intern("x"));

    TEST_ASSERT(t != NULL);
    TEST_ASSERT(cursor != NULL);
    TEST_ASSERT(cursor->type == TOKEN_EOF);

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
    TEST_ASSERT(cursor->type == TOKEN_EOF);

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
    TEST_ASSERT(cursor->type == TOKEN_EOF);

    term_destroy(t);
    lex_destroy_tokens(tokens);
    return PASSED;
}
