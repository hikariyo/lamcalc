#include "lexer.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static int _is_name(char ch) {
    return !(isspace(ch) || ch == '(' || ch == ')' || ch == '\\' || ch == '.');
}

static token_t *_create_token(sym_t sym, token_type_t type, token_t *next) {
    token_t *token = (token_t *)malloc(sizeof(token_t));
    assert(token != NULL);
    token->sym = sym;
    token->type = type;
    token->next = next;
    return token;
}

token_t *lex_string(const char *str) {
    size_t len = strlen(str);

    token_t dummy_head;
    token_t *tail_prev = &dummy_head;
    tail_prev->next = NULL;

    char buf[SYM_MAXLEN + 1];

    for (size_t i = 0; i < len; i++) {
        char now = str[i];
        if (isspace(now)) {
            continue;
        }

        token_t *tail = tail_prev->next;
        if (now == '(') {
            tail_prev->next = _create_token(-1, TOKEN_LP, tail);
        } else if (now == ')') {
            tail_prev->next = _create_token(-1, TOKEN_RP, tail);
        } else if (now == '\\') {
            tail_prev->next = _create_token(-1, TOKEN_LAMBDA, tail);
        } else if (now == '.') {
            tail_prev->next = _create_token(-1, TOKEN_DOT, tail);
        } else {
            size_t j = i;
            while (j < len && _is_name(str[j])) {
                j++;
            }

            assert(j - i <= SYM_MAXLEN);
            strncpy(buf, &str[i], j - i);
            buf[j - i] = '\0';
            i = j - 1;

            tail_prev->next = _create_token(sym_intern(buf), TOKEN_NAME, tail);
        }

        tail_prev = tail_prev->next;
    }

    return dummy_head.next;
}

void lex_destroy_tokens(token_t *tokens) {
    token_t *p = tokens;
    while (p != NULL) {
        token_t *next = p->next;
        free(p);
        p = next;
    }
}
