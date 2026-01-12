#include "term.h"
#include "symbol.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

term_t *term_copy(const term_t *term) {
    switch (term->type) {
    case TM_VAR:
        return term_var(term->data.var.sym, term->data.var.index);
    case TM_ABS:
        return term_abs(term->data.abs.param, term_copy(term->data.abs.body));
    case TM_APP:
        return term_app(term_copy(term->data.app.left),
                        term_copy(term->data.app.right));
    }
}

static void _shift(term_t *term, int delta, int cutoff) {
    switch (term->type) {
    case TM_APP:
        _shift(term->data.app.left, delta, cutoff);
        _shift(term->data.app.right, delta, cutoff);
        break;
    case TM_VAR:
        if (term->data.var.index >= cutoff) {
            term->data.var.index += delta;
        }
        break;
    case TM_ABS:
        _shift(term->data.abs.body, delta, cutoff + 1);
        break;
    }
}

// Consumes 'term'.
static term_t *_subst(term_t *term, int index, const term_t *val) {
    switch (term->type) {
    case TM_VAR: {
        if (term->data.var.index == index) {
            term_t *ret = term_copy(val);
            _shift(ret, index, 0);
            term_destroy(term);
            return ret;
        } else if (term->data.var.index > index) {
            term->data.var.index--;
        }
        return term;
    }
    case TM_ABS: {
        term->data.abs.body = _subst(term->data.abs.body, index + 1, val);
        return term;
    }
    case TM_APP: {
        term->data.app.left = _subst(term->data.app.left, index, val);
        term->data.app.right = _subst(term->data.app.right, index, val);
        return term;
    }
    }
}

static term_t *_new_term(term_type_t type) {
    term_t *term = (term_t *)malloc(sizeof(term_t));
    assert(term != NULL);
    term->type = type;
    return term;
}

static term_t *_eval_lim_depth_lazy(term_t *term, int depth, int *steps) {
    if (depth > TERM_EVAL_MAX_DEPTH) {
        term_destroy(term);
        printf("error: max recursion depth exceeded(possible infinite "
               "recursion)\n");
        return NULL;
    }

    for (;;) {
        if (term->type != TM_APP) {
            return term;
        }

        term_t *left =
            _eval_lim_depth_lazy(term->data.app.left, depth + 1, steps);
        term_t *right = term->data.app.right;
        free(term);
        if (left == NULL) {
            term_destroy(right);
            return NULL;
        }

        if (left->type == TM_ABS) {
            if (steps != NULL) {
                *steps = *steps + 1;
            }
            term_t *body = _subst(left->data.abs.body, 0, right);
            free(left);
            term_destroy(right);

            term = body;
        } else {
            return term_app(left, right);
        }
    }
}

term_t *term_eval_steps(term_t *term, int *steps) {
    sym_t f = sym_intern("f");
    sym_t x = sym_intern("x");
    sym_t a = sym_intern("a");
    sym_t b = sym_intern("b");

    sym_t n = sym_intern("n");
    sym_t g = sym_intern("g");
    sym_t h = sym_intern("h");
    sym_t u = sym_intern("u");
    sym_t v = sym_intern("v");
    sym_t p = sym_intern("p");

    term_t *plus = term_abs(
        a, term_abs(
               b, term_abs(
                      f, term_abs(x, term_app(term_app(term_var(b, 2),
                                                       term_var(f, 1)),
                                              term_app(term_app(term_var(a, 3),
                                                                term_var(f, 1)),
                                                       term_var(x, 0)))))));
    term_t *mul = term_abs(
        a, term_abs(b, term_abs(f, term_app(term_var(a, 2),
                                            term_app(term_var(b, 1),
                                                     term_var(f, 0))))));

    term_t *Yl = term_abs(
        x, term_app(term_var(f, 1), term_app(term_var(x, 0), term_var(x, 0))));
    term_t *Yr = term_copy(Yl);
    term_t *Y = term_abs(f, term_app(Yl, Yr));

    term_t *True = term_abs(a, term_abs(b, term_var(a, 1)));
    term_t *False = term_abs(a, term_abs(b, term_var(b, 0)));

    term_t *pred = term_abs(
        n,
        term_abs(
            f,
            term_abs(
                x, term_app(
                       term_app(
                           term_app(
                               term_var(n, 2), // n
                               term_abs(
                                   g, // step function
                                   term_abs(
                                       h, term_app(term_var(h, 0),
                                                   term_app(term_var(g, 1),
                                                            term_var(f, 3)))))),
                           term_abs(u, term_var(x, 1)) // initial value
                           ),
                       term_abs(v, term_var(v, 0)) // extract result
                       ))));
    term_t *iszero = term_abs(
        n, term_app(term_app(term_var(n, 0), term_abs(x, term_copy(False))),
                    term_copy(True)));

    term_t *cons = term_abs(
        a, term_abs(
               b, term_abs(f, term_app(term_app(term_var(f, 0), term_var(a, 2)),
                                       term_var(b, 1)))));

    term_t *head = term_abs(
        p, term_app(term_var(p, 0), term_abs(a, term_abs(b, term_var(a, 1)))));

    term_t *tail = term_abs(
        p, term_app(term_var(p, 0), term_abs(a, term_abs(b, term_var(b, 0)))));

    term_t *nil = term_abs(a, term_abs(b, term_var(b, 0)));

    term_t *isnil = term_abs(
        p,
        term_app(
            term_app(term_var(p, 0),
                     term_abs(a, term_abs(b, term_abs(x, term_copy(False))))),
            term_copy(True)));

    // term_t* parse(token_t** tokens, token_t* end)
    // should be kept updated with this
    term = term_app(term_abs(sym_intern("isnil"), term), isnil);
    term = term_app(term_abs(sym_intern("nil"), term), nil);
    term = term_app(term_abs(sym_intern("tail"), term), tail);
    term = term_app(term_abs(sym_intern("head"), term), head);
    term = term_app(term_abs(sym_intern("cons"), term), cons);
    term = term_app(term_abs(sym_intern("iszero"), term), iszero);
    term = term_app(term_abs(sym_intern("pred"), term), pred);
    term = term_app(term_abs(sym_intern("false"), term), False);
    term = term_app(term_abs(sym_intern("true"), term), True);
    term = term_app(term_abs(sym_intern("Y"), term), Y);
    term = term_app(term_abs(sym_intern("+"), term), plus);
    term = term_app(term_abs(sym_intern("*"), term), mul);

    return _eval_lim_depth_lazy(term, 0, steps);
}

term_t *term_var(sym_t sym, int index) {
    term_t *term = _new_term(TM_VAR);
    term->data.var.sym = sym;
    term->data.var.index = index;
    return term;
}

term_t *term_abs(sym_t param, term_t *body) {
    term_t *term = _new_term(TM_ABS);
    term->data.abs.param = param;
    term->data.abs.body = body;
    return term;
}

term_t *term_app(term_t *left, term_t *right) {
    term_t *term = _new_term(TM_APP);
    term->data.app.left = left;
    term->data.app.right = right;
    return term;
}

void term_destroy(term_t *term) {
    switch (term->type) {
    case TM_VAR:
        break;
    case TM_ABS:
        term_destroy(term->data.abs.body);
        break;
    case TM_APP:
        term_destroy(term->data.app.left);
        term_destroy(term->data.app.right);
        break;
    }
    free(term);
}

static size_t _repr_size_t_size(size_t x) {
    if (x == 0) {
        return 1;
    }
    size_t res = 0;
    while (x) {
        res++;
        x /= 10;
    }
    return res;
}

static size_t _repr_size(const term_t *term) {
    int church = term_as_church(term);
    if (church != TERM_INVALID_CHURCH) {
        return _repr_size_t_size(church);
    }

    switch (term->type) {
    case TM_VAR:
        // name
        return strlen(sym_name(term->data.var.sym));
    case TM_ABS:
        // (\\param.body)
        return 4 + strlen(sym_name(term->data.abs.param)) +
               _repr_size(term->data.abs.body);
    case TM_APP:
        // (left right)
        return 3 + _repr_size(term->data.app.left) +
               _repr_size(term->data.app.right);
    }
}

static void _repr(const term_t *term, char **p) {
    int church = term_as_church(term);
    if (church != TERM_INVALID_CHURCH) {
        // The buffer size is calculated in advance.
        int x = snprintf(*p, INT_MAX, "%d", church);
        assert(x > 0);
        (*p) += x;
        return;
    }

    char *now = *p;
    switch (term->type) {
    case TM_VAR: {
        const char *name = sym_name(term->data.var.sym);
        while (*name != '\0') {
            *now = *name;
            name++;
            now++;
        }

        break;
    }
    case TM_APP: {
        *now = '(';
        now++;

        _repr(term->data.app.left, &now);

        *now = ' ';
        now++;

        _repr(term->data.app.right, &now);

        *now = ')';
        now++;
        break;
    }
    case TM_ABS: {
        *now = '(';
        now++;

        *now = '\\';
        now++;

        const char *name = sym_name(term->data.abs.param);
        while (*name != '\0') {
            *now = *name;
            name++;
            now++;
        }

        *now = '.';
        now++;

        _repr(term->data.abs.body, &now);

        *now = ')';
        now++;
    }
    }
    *p = now;
}

char *term_repr(const term_t *term) {
    size_t sz = _repr_size(term) + 1;
    char *res = malloc(sizeof(char) * sz);
    char *p = res;
    assert(res != NULL);
    _repr(term, &p);
    *p = '\0';
    return res;
}

int term_as_church(const term_t *term) {
    term_t *t = term_copy(term);
    term_t *outer = NULL;

    if (t == NULL) {
        return TERM_INVALID_CHURCH;
    }

    t = _eval_lim_depth_lazy(t, 0, NULL);
    if (t->type != TM_ABS) {
        term_destroy(t);
        return TERM_INVALID_CHURCH;
    }

    outer = t;
    t = t->data.abs.body;
    free(outer);
    t = _eval_lim_depth_lazy(t, 0, NULL);

    if (t->type != TM_ABS) {
        term_destroy(t);
        return TERM_INVALID_CHURCH;
    }

    outer = t;
    t = t->data.abs.body;
    free(outer);
    t = _eval_lim_depth_lazy(t, 0, NULL);

    int res = 0;
    while (t != NULL) {
        if (t->type == TM_VAR && t->data.var.index == 0) {
            term_destroy(t);
            break;
        }

        if (t->type != TM_APP || t->data.app.left->type != TM_VAR ||
            t->data.app.left->data.var.index != 1) {
            term_destroy(t);
            return TERM_INVALID_CHURCH;
        }

        outer = t;
        t = t->data.app.right;
        term_destroy(outer->data.app.left);
        free(outer);
        res++;
        t = _eval_lim_depth_lazy(t, 0, NULL);
    }

    return res;
}

term_t *term_from_church(int num) {
    static int cnt;
    char buf[SYM_MAXLEN + 1];
    cnt++;
    snprintf(buf, SYM_MAXLEN, "$c%df", cnt);
    sym_t f = sym_intern(buf);
    snprintf(buf, SYM_MAXLEN, "$c%dx", cnt);
    sym_t x = sym_intern(buf);

    term_t *t = term_var(x, 0);
    for (int i = 0; i < num; i++) {
        t = term_app(term_var(f, 1), t);
    }
    return term_abs(f, term_abs(x, t));
}
