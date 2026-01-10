#include "term.h"
#include <assert.h>
#include <stdlib.h>

static term_t *substitute(term_t *term, sym_t name, term_t *val,
                          int *referred_val) {
    switch (term->type) {
    case TM_VAR:
        if (term->data.var == name) {
            free(term);
            *referred_val = 1;
            return val;
        } else {
            return term;
        }
    case TM_ABS: {
        sym_t param = term->data.abs.param;
        term_t *body = term->data.abs.body;
        free(term);
        return term_abs(param, substitute(body, name, val, referred_val));
    }
    case TM_APP: {
        term_t *left = term->data.app.left;
        term_t *right = term->data.app.right;
        free(term);
        return term_app(substitute(left, name, val, referred_val),
                        substitute(right, name, val, referred_val));
    }
    }
}

static term_t *create_term(term_type_t type) {
    term_t *term = (term_t *)malloc(sizeof(term_t));
    assert(term != NULL);
    term->type = type;
    return term;
}

term_t *term_eval(term_t *term) {
    switch (term->type) {
    case TM_VAR:
    case TM_ABS:
        return term;
    case TM_APP: {
        term_t *left = term_eval(term->data.app.left);
        term_t *right = term_eval(term->data.app.right);
        free(term);

        if (left->type == TM_ABS) {
            int refereed_right = 0;
            term_t *body = substitute(left->data.abs.body, left->data.abs.param,
                                      right, &refereed_right);
            free(left);
            if (!refereed_right) {
                free(right);
            }
            return term_eval(body);
        }

        return term_app(left, right);
    }
    }
}

term_t *term_var(sym_t var) {
    term_t *term = create_term(TM_VAR);
    term->data.var = var;
    return term;
}

term_t *term_abs(sym_t param, term_t *body) {
    term_t *term = create_term(TM_ABS);
    term->data.abs.param = param;
    term->data.abs.body = body;
    return term;
}

term_t *term_app(term_t *left, term_t *right) {
    term_t *term = create_term(TM_APP);
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
