#include "parser.h"
#include "lexer.h"
#include "symbol.h"
#include "term.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int _num_abs;

struct _stk {
    sym_t raw;
    sym_t rep;
    struct _stk *prev;
};

static struct _stk *_tail;

static void _add_size_t(char *s, size_t x) {
    size_t curlen = strlen(s);
    size_t remaining_space = SYM_MAXLEN + 1 - curlen;
    int written = snprintf(s + curlen, remaining_space, "%zu", x);
    assert(!(written < 0 || (size_t)written >= remaining_space));
}

static void _rename_sym(sym_t raw) {
    struct _stk *now = malloc(sizeof(struct _stk));
    assert(now != NULL);
    now->raw = raw;

    const char *raws = sym_name(raw);
    char reps[SYM_MAXLEN + 1];
    strcpy(reps, raws);
    _add_size_t(reps, _num_abs);
    now->rep = sym_intern(reps);

    now->prev = _tail;
    _tail = now;
}

static sym_t _find_sym(sym_t sym) {
    sym_t ret = -1;
    int cnt = 0;
    for (struct _stk *p = _tail; p != NULL; p = p->prev) {
        if (p->raw == sym) {
            if (cnt >= 1) {
                return ret;
            }

            cnt++;
            ret = p->rep;
        }
    }

    // Freevar or cnt == 1
    return sym;
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

    _rename_sym(t->sym);
    sym_t param = _find_sym(t->sym);

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
            return -1;
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
        if (ival != -1) {
            return term_from_church(ival);
        }
        return term_var(_find_sym(t->sym));
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
        _num_abs++;
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
    _num_abs = 0;
    _tail = NULL;
    term_t *term = _parse(tokens, end);
    while (_tail != NULL) {
        struct _stk *prev = _tail->prev;
        free(_tail);
        _tail = prev;
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
