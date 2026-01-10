#include "lexer.h"
#include "symbol.h"
#include "test.h"
#include <string.h>

#define ASSERT_TOKEN(t, expected_type)                                         \
    TEST_ASSERT((t) != NULL);                                                  \
    TEST_ASSERT((t)->type == (expected_type))

#define ASSERT_TOKEN_NAME(t, expected_name)                                    \
    ASSERT_TOKEN(t, TOKEN_NAME);                                               \
    TEST_ASSERT(!strcmp(sym_name((t)->sym), (expected_name)))

TEST(lexer_basic) {
    const char *source = "\\.()";
    token_t *tokens = lex_string(source);

    token_t *t = tokens;
    ASSERT_TOKEN(t, TOKEN_LAMBDA);

    t = t->next;
    ASSERT_TOKEN(t, TOKEN_DOT);

    t = t->next;
    ASSERT_TOKEN(t, TOKEN_LP);

    t = t->next;
    ASSERT_TOKEN(t, TOKEN_RP);

    t = t->next;
    TEST_ASSERT(t == NULL);

    lex_destroy_tokens(tokens);
    return PASSED;
}

TEST(lexer_lambda_expr) {
    const char *source = "\\x. x y";
    token_t *tokens = lex_string(source);

    token_t *t = tokens;
    ASSERT_TOKEN(t, TOKEN_LAMBDA);

    t = t->next;
    ASSERT_TOKEN_NAME(t, "x");

    t = t->next;
    ASSERT_TOKEN(t, TOKEN_DOT);

    t = t->next;
    ASSERT_TOKEN_NAME(t, "x");

    t = t->next;
    ASSERT_TOKEN_NAME(t, "y");

    t = t->next;
    TEST_ASSERT(t == NULL);

    lex_destroy_tokens(tokens);
    return PASSED;
}

TEST(lexer_spacing) {
    const char *source = "  \\   x   .x  ";
    token_t *tokens = lex_string(source);

    token_t *t = tokens;
    ASSERT_TOKEN(t, TOKEN_LAMBDA);

    t = t->next;
    ASSERT_TOKEN_NAME(t, "x");

    t = t->next;
    ASSERT_TOKEN(t, TOKEN_DOT);

    t = t->next;
    ASSERT_TOKEN_NAME(t, "x");

    t = t->next;
    TEST_ASSERT(t == NULL);

    lex_destroy_tokens(tokens);
    return PASSED;
}