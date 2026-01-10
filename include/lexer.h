#ifndef LEXER_H
#define LEXER_H

#include "symbol.h"

typedef enum {
    TOKEN_NAME,
    TOKEN_LAMBDA,
    TOKEN_DOT,
    TOKEN_LP,
    TOKEN_RP,
    TOKEN_EOF,
} token_type_t;

typedef struct token {
    sym_t sym;
    token_type_t type;
    struct token *next;
} token_t;

token_t *lex_string(const char *str);

void lex_destroy_tokens(token_t *tokens);

#endif // LEXER_H