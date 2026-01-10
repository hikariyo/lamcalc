#ifndef LEXER_H
#define LEXER_H

#include "symbol.h"

typedef enum {
    TOKEN_NAME,   // Identifier (e.g., x, y, var_name)
    TOKEN_LAMBDA, // Lambda symbol '\'
    TOKEN_DOT,    // Function body separator '.'
    TOKEN_LP,     // Left parenthesis '('
    TOKEN_RP,     // Right parenthesis ')'
} token_type_t;

typedef struct token {
    token_type_t type;
    sym_t sym;          // Associated symbol (only valid for TOKEN_NAME)
    struct token *next; // Pointer to the next token in the stream
} token_t;

// Lexes the input string and produces a linked-list of tokens.
// Returns owned pointer as the head of the token stream.
token_t *lex_string(const char *str);

// Destroys the token stream. Consumes 'tokens'.
void lex_destroy_tokens(token_t *tokens);

#endif // LEXER_H