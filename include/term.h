#ifndef VARSTR_H
#define VARSTR_H

#include "symbol.h"

typedef enum {
    TM_VAR,
    TM_ABS,
    TM_APP,
} term_type_t;

typedef struct term term_t;

typedef struct {
    sym_t param;
    term_t *body;
} term_abs_t;

typedef struct {
    term_t *left;
    term_t *right;
} term_app_t;

struct term {
    term_type_t type;
    union {
        sym_t var;
        term_abs_t abs;
        term_app_t app;
    } data;
};

term_t *term_new_var(sym_t var);

term_t *term_new_abs(sym_t param, term_t *body);

term_t *term_new_app(term_t *left, term_t *right);

void term_destroy(term_t *term);

term_t *term_eval(term_t *term);

#endif // VARSTR_H