#include "term.h"
#include "symbol.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static term_t *_copy(const term_t *term) {
    switch (term->type) {
    case TM_VAR:
        return term_var(term->data.var.sym, term->data.var.index);
    case TM_ABS:
        return term_abs(term->data.abs.param, _copy(term->data.abs.body));
    case TM_APP:
        return term_app(_copy(term->data.app.left),
                        _copy(term->data.app.right));
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
        term_t *ret = NULL;
        if (term->data.var.index == index) {
            ret = _copy(val);
            _shift(ret, index, 0);
        } else {
            ret = _copy(term);
            if (ret->data.var.index > index) {
                ret->data.var.index--;
            }
        }
        term_destroy(term);
        return ret;
    }
    case TM_ABS: {
        sym_t param = term->data.abs.param;
        term_t *body = _subst(term->data.abs.body, index + 1, val);
        free(term);
        return term_abs(param, body);
    }
    case TM_APP: {
        term_t *left = _subst(term->data.app.left, index, val);
        term_t *right = _subst(term->data.app.right, index, val);
        free(term);
        return term_app(left, right);
    }
    }
}

static term_t *_new_term(term_type_t type) {
    term_t *term = (term_t *)malloc(sizeof(term_t));
    assert(term != NULL);
    term->type = type;
    return term;
}

// Consumes 'term'.
static term_t *_eval_lim_depth(term_t *term, int depth) {
    if (depth > TERM_EVAL_MAX_DEPTH) {
        term_destroy(term);
        printf("error: max recursion depth exceeded(possible infinite "
               "recursion)\n");
        return NULL;
    }

    switch (term->type) {
    case TM_VAR: {
        term_t *ret = _copy(term);
        term_destroy(term);
        return ret;
    }
    case TM_ABS: {
        term_t *body = _eval_lim_depth(term->data.abs.body, depth + 1);
        if (body == NULL) {
            free(term);
            return NULL;
        }

        sym_t param = term->data.abs.param;
        free(term);
        return term_abs(param, body);
    }
    case TM_APP: {
        term_t *left = _eval_lim_depth(term->data.app.left, depth + 1);
        if (left == NULL) {
            term_destroy(term->data.app.right);
            free(term);
            return NULL;
        }

        term_t *right = _eval_lim_depth(term->data.app.right, depth + 1);
        if (right == NULL) {
            term_destroy(left);
            free(term);
            return NULL;
        }

        free(term);
        if (left->type == TM_ABS) {
            term_t *body = _subst(left->data.abs.body, 0, right);
            free(left);
            term_destroy(right);
            return _eval_lim_depth(body, depth + 1);
        }

        return term_app(left, right);
    }
    }
}

term_t *term_fuck(term_t *term) { return _eval_lim_depth(term, 0); }

term_t *term_eval(term_t *term) {
    sym_t f = sym_intern("f");
    sym_t x = sym_intern("x");
    sym_t a = sym_intern("a");
    sym_t b = sym_intern("b");

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

    term = term_app(term_abs(sym_intern("+"), term), plus);
    term = term_app(term_abs(sym_intern("*"), term), mul);
    return _eval_lim_depth(term, 0);
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

static size_t _repr_size(const term_t *term) {
    int church = term_as_church(term);
    if (church != TERM_INVALID_CHURCH) {
        if (church == 0) {
            return 1;
        }
        int res = 0;
        while (church) {
            res++;
            church /= 10;
        }
        return res;
    }

    switch (term->type) {
    case TM_VAR:
        return strlen(sym_name(term->data.var.sym));
    case TM_ABS:
        return 2 + strlen(sym_name(term->data.abs.param)) +
               _repr_size(term->data.abs.body);
    case TM_APP:
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
    if (term->type != TM_ABS) {
        return TERM_INVALID_CHURCH;
    }

    term = term->data.abs.body;
    if (term->type != TM_ABS) {
        return TERM_INVALID_CHURCH;
    }

    term = term->data.abs.body;

    int res = 0;
    while (term != NULL) {
        if (term->type == TM_VAR && term->data.var.index == 0) {
            break;
        }

        if (term->type != TM_APP || term->data.app.left->type != TM_VAR ||
            term->data.app.left->data.var.index != 1) {
            return TERM_INVALID_CHURCH;
        }

        res++;
        term = term->data.app.right;
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
