#include "term.h"
#include "symbol.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static term_t *copyterm(const term_t *term) {
    switch (term->type) {
    case TM_VAR:
        return term_var(term->data.var);
    case TM_ABS:
        return term_abs(term->data.abs.param, copyterm(term->data.abs.body));
    case TM_APP:
        return term_app(copyterm(term->data.app.left),
                        copyterm(term->data.app.right));
    }
}

static term_t *substitute(term_t *term, sym_t name, const term_t *val) {
    switch (term->type) {
    case TM_VAR:
        if (term->data.var == name) {
            free(term);
            return copyterm(val);
        } else {
            return term;
        }
    case TM_ABS: {
        sym_t param = term->data.abs.param;
        term_t *body = term->data.abs.body;
        free(term);
        return term_abs(param, substitute(body, name, val));
    }
    case TM_APP: {
        term_t *left = term->data.app.left;
        term_t *right = term->data.app.right;
        free(term);
        return term_app(substitute(left, name, val),
                        substitute(right, name, val));
    }
    }
}

static term_t *create_term(term_type_t type) {
    term_t *term = (term_t *)malloc(sizeof(term_t));
    assert(term != NULL);
    term->type = type;
    return term;
}

static term_t *eval_limited_recur_depth(term_t *term, int depth) {
    if (depth > TERM_EVAL_MAX_DEPTH) {
        printf("error: max recursion depth exceeded(possible infinite "
               "recursion).\n");
        return NULL;
    }

    switch (term->type) {
    case TM_VAR:
    case TM_ABS:
        return term;
    case TM_APP: {
        term_t *left = eval_limited_recur_depth(term->data.app.left, depth + 1);
        if (left == NULL) {
            return NULL;
        }
        term_t *right =
            eval_limited_recur_depth(term->data.app.right, depth + 1);

        if (right == NULL) {
            return NULL;
        }

        free(term);
        if (left->type == TM_ABS) {
            term_t *body =
                substitute(left->data.abs.body, left->data.abs.param, right);
            free(left);
            free(right);
            return eval_limited_recur_depth(body, depth + 1);
        }

        return term_app(left, right);
    }
    }
}

term_t *term_eval(term_t *term) { return eval_limited_recur_depth(term, 0); }

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

static size_t repr_size(const term_t *term) {
    switch (term->type) {
    case TM_VAR:
        return strlen(sym_name(term->data.var));
    case TM_ABS:
        return 2 + strlen(sym_name(term->data.abs.param)) +
               repr_size(term->data.abs.body);
    case TM_APP:
        return 3 + repr_size(term->data.app.left) +
               repr_size(term->data.app.right);
    }
}

static void repr(const term_t *term, char **p) {
    char *now = *p;
    switch (term->type) {
    case TM_VAR: {
        const char *name = sym_name(term->data.var);
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

        repr(term->data.app.left, &now);

        *now = ' ';
        now++;

        repr(term->data.app.right, &now);

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

        repr(term->data.abs.body, &now);
    }
    }
    *p = now;
}

char *term_repr(const term_t *term) {
    size_t sz = repr_size(term) + 1;
    char *res = malloc(sizeof(char) * sz);
    char *p = res;
    assert(res != NULL);
    repr(term, &p);
    *p = '\0';
    return res;
}
