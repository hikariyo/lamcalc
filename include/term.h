#ifndef TERM_H
#define TERM_H

#include "symbol.h"
#include <stddef.h>

typedef enum {
    TM_VAR,
    TM_ABS,
    TM_APP,
} term_type_t;

typedef struct term term_t;

typedef struct {
    sym_t param;
    term_t *body; // Owned
} term_abs_t;

typedef struct {
    term_t *left;  // Owned
    term_t *right; // Owned
} term_app_t;

struct term {
    term_type_t type;
    union {
        sym_t var;
        term_abs_t abs;
        term_app_t app;
    } data;
};

// Creates a variable term.
term_t *term_var(sym_t var);

// Creates an abstraction term.
// Takes ownership of body.
term_t *term_abs(sym_t param, term_t *body);

// Creates an application term.
// Takes ownership of left and right.
term_t *term_app(term_t *left, term_t *right);

// Recursively frees the term and all its children.
void term_destroy(term_t *term);

// Evaluates given term (Reduction).
// Note: This function consumes the input 'term'. Use the returned pointer only.
// The input 'term' may be modified in-place or freed.
term_t *term_eval(term_t *term);

// Gets a string representation of the term.
// Caller takes the ownership of returned string.
char *term_repr(const term_t *term);

#endif // TERM_H