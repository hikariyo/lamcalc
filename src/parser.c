#include "parser.h"
#include "lexer.h"
#include "term.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static term_t *_parse_abs(token_t **tokens, token_t *end) {
    token_t *t = *tokens;
    assert(t->type == TOKEN_LAMBDA);

    t = t->next;
    assert(t->type == TOKEN_NAME);
    const char *param = t->sym;

    t = t->next;
    assert(t->type == TOKEN_DOT);

    t = t->next;
    term_t *body = parse(&t, end);

    *tokens = t;
    return term_abs(param, body);
}

static token_t *_find_rp(token_t *t) {
    int now = 0;
    for (token_t *p = t; p != NULL; p = p->next) {
        if (p->type == TOKEN_LP) {
            now++;
        } else if (p->type == TOKEN_RP) {
            now--;
        }
        if (now == 0) {
            return p;
        }
    }
    return NULL;
}

static term_t *_parse_atom(token_t **tokens) {
    token_t *t = *tokens;
    switch (t->type) {
    case TOKEN_NAME:
        *tokens = t->next;
        return term_var(t->sym);
    case TOKEN_LP: {
        t = t->next;
        token_t *rp = _find_rp(*tokens);
        if (rp == NULL) {
            printf("error: no matched right parenthesis\n");
            return NULL;
        }
        term_t *res = parse(&t, rp);
        assert(t->type == TOKEN_RP);
        *tokens = t->next;
        return res;
    }
    default:
        printf("error: unexpected token\n");
        return NULL;
    }
}

term_t *parse(token_t **tokens, token_t *end) {
    if (*tokens == NULL) {
        printf("error: unexpected null\n");
        return NULL;
    }

    switch ((*tokens)->type) {
    case TOKEN_LAMBDA:
        return _parse_abs(tokens, end);
    case TOKEN_NAME:
    case TOKEN_LP: {
        term_t *now = NULL;
        while (*tokens != end) {
            term_t *atom = _parse_atom(tokens);

            // A NULL returned from parse_atom signals a syntax error.
            if (atom == NULL) {
                return NULL;
            }

            // Reduction.
            now = now == NULL ? atom : term_app(now, atom);
        }
        assert(now != NULL);
        return now;
    }
    default:
        printf("error: unexpected token\n");
        return NULL;
    }
}
