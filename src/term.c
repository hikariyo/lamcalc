#include "term.h"
#include <assert.h>
#include <stdlib.h>

static term_t *substitute(term_t *term, sym_t name, term_t *val) {
    switch (term->type) {
    case TM_VAR:
        if (term->data.var == name)
            return val;
        else
            return term;
    case TM_ABS:
        return term_new_abs(term->data.abs.param,
                            substitute(term->data.abs.body, name, val));
    case TM_APP:
        return term_new_app(substitute(term->data.app.left, name, val),
                            substitute(term->data.app.right, name, val));
    }
}

static term_t *create_term(term_type_t type) {
    // TODO: garbage collection
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

        if (left->type == TM_ABS) {
            term_t *body =
                substitute(left->data.abs.body, left->data.abs.param, right);
            return term_eval(body);
        }

        return term_new_app(left, right);
    }
    }
}

term_t *term_new_var(sym_t var) {
    term_t *term = create_term(TM_VAR);
    term->data.var = var;
    return term;
}

term_t *term_new_abs(sym_t param, term_t *body) {
    term_t *term = create_term(TM_ABS);
    term->data.abs.param = param;
    term->data.abs.body = body;
    return term;
}

term_t *term_new_app(term_t *left, term_t *right) {
    term_t *term = create_term(TM_APP);
    term->data.app.left = left;
    term->data.app.right = right;
    return term;
}
