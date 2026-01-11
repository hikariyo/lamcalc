#include "parser.h"
#include "lexer.h"
#include "symbol.h"
#include "term.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _INVALID_INT -1

typedef struct sym_stack {
    sym_t sym;
    struct sym_stack *prev;
} sym_stack_t;

static sym_stack_t *_tail;

static void _push_stack(sym_t sym) {
    sym_stack_t *tail = (sym_stack_t *)malloc(sizeof(sym_stack_t));
    assert(tail != NULL);
    tail->prev = _tail;
    tail->sym = sym;
    _tail = tail;
}

static void _pop_stack() {
    sym_stack_t *prev = _tail->prev;
    free(_tail);
    _tail = prev;
}

static int _find_stack_index(sym_t sym) {
    int res = 0;
    for (sym_stack_t *p = _tail; p != NULL; p = p->prev) {
        if (p->sym == sym) {
            return res;
        }
        res++;
    }
    return TERM_VAR_INVALID_INDEX;
}

static term_t *_parse(token_t **tokens, token_t *end);

static term_t *_parse_abs(token_t **tokens, token_t *end) {
    token_t *t = *tokens;

    if (t == NULL) {
        printf("error: unexpected null in rule AbsTerm; expected NAME\n");
        return NULL;
    } else if (t->type != TOKEN_LAMBDA) {
        printf("error: unexpected token %s in rule AbsTerm; expected LAMBDA\n",
               lex_token_type_repr(t->type));
        return NULL;
    }

    t = t->next;

    if (t == NULL) {
        printf("error: unexpected null in rule AbsTerm; expected NAME\n");
        return NULL;
    } else if (t->type != TOKEN_NAME) {
        printf("error: unexpected token %s in rule AbsTerm; expected NAME\n",
               lex_token_type_repr(t->type));
        return NULL;
    }

    _push_stack(t->sym);
    sym_t param = t->sym;

    t = t->next;
    if (t == NULL) {
        printf("error: unexpected null in rule AbsTerm; expected DOT\n");
        return NULL;
    } else if (t->type != TOKEN_DOT) {
        printf("error: unexpected token %s in rule AbsTerm; expected DOT\n",
               lex_token_type_repr(t->type));
        return NULL;
    }

    t = t->next;
    term_t *body = _parse(&t, end);
    _pop_stack();
    // Body will be null on syntax error.
    if (body == NULL) {
        return NULL;
    }

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

static int _convert_to_int(const char *s) {
    int res = 0;
    for (; *s; s++) {
        if (!isdigit(*s)) {
            return _INVALID_INT;
        }
        res = res * 10 + *s - '0';
    }
    return res;
}

static term_t *_parse_atom(token_t **tokens) {
    token_t *t = *tokens;
    switch (t->type) {
    case TOKEN_NAME: {
        *tokens = t->next;
        int ival = _convert_to_int(sym_name(t->sym));
        if (ival != _INVALID_INT) {
            return term_from_church(ival);
        }
        return term_var(t->sym, _find_stack_index(t->sym));
    }
    case TOKEN_LP: {
        t = t->next;
        token_t *rp = _find_rp(*tokens);
        if (rp == NULL) {
            printf("error: no matched right parenthesis in rule Atom\n");
            return NULL;
        }
        term_t *res = _parse(&t, rp);
        if (res == NULL) {
            return NULL;
        }
        assert(t->type == TOKEN_RP);
        *tokens = t->next;
        return res;
    }
    default:
        printf("error: unexpected token %s in rule Atom; expected NAME | LP\n",
               lex_token_type_repr(t->type));
        return NULL;
    }
}

static term_t *_parse(token_t **tokens, token_t *end) {
    if (*tokens == NULL) {
        printf("error: unexpected null in rule Term; expected LAMBDA | NAME | "
               "LP\n");
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
                if (now != NULL) {
                    term_destroy(now);
                }
                return NULL;
            }

            // Reduction.
            now = now == NULL ? atom : term_app(now, atom);
        }
        assert(now != NULL);
        return now;
    }
    default:
        printf("error: unexpected token %s in rule Term; expected LAMBDA | "
               "NAME | LP\n",
               lex_token_type_repr((*tokens)->type));
        return NULL;
    }
}

term_t *parse(token_t **tokens, token_t *end) {
    _tail = NULL;

    _push_stack(sym_intern("*"));
    _push_stack(sym_intern("+"));
    _push_stack(sym_intern("Y"));
    _push_stack(sym_intern("true"));
    _push_stack(sym_intern("false"));
    _push_stack(sym_intern("pred"));
    _push_stack(sym_intern("iszero"));
    _push_stack(sym_intern("cons"));
    _push_stack(sym_intern("head"));
    _push_stack(sym_intern("tail"));
    _push_stack(sym_intern("nil"));
    _push_stack(sym_intern("isnil"));

    term_t *term = _parse(tokens, end);
    while (_tail != NULL) {
        _pop_stack();
    }
    return term;
}

term_t *parse_string(const char *str) {
    token_t *tokens = lex_string(str);
    token_t *cursor = tokens;
    term_t *term = parse(&cursor, NULL);
    lex_destroy_tokens(tokens);
    return term;
}
