#ifndef TERM_H
#define TERM_H

#include "symbol.h"
#include <stddef.h>

#define TERM_EVAL_MAX_DEPTH 1024
#define TERM_VAR_INVALID_INDEX -998244353
#define TERM_INVALID_CHURCH -1

typedef enum {
    TM_VAR,
    TM_ABS,
    TM_APP,
} term_type_t;

typedef struct term term_t;

typedef struct {
    sym_t param;  // For debug only
    term_t *body; // Owned
} term_abs_t;

typedef struct {
    term_t *left;  // Owned
    term_t *right; // Owned
} term_app_t;

typedef struct {
    sym_t sym; // For debug only
    int index;
} term_var_t;

struct term {
    term_type_t type;
    union {
        term_var_t var;
        term_abs_t abs;
        term_app_t app;
    } data;
};

// Creates a variable term.
// Returns an owned pointer.
term_t *term_var(sym_t sym, int index);

// Creates an abstraction. Consumes 'body'.
// Returns an owned pointer.
term_t *term_abs(sym_t param, term_t *body);

// Creates an application. Consumes 'left' and 'right'.
// Returns an owned pointer.
term_t *term_app(term_t *left, term_t *right);

// Destroys the term and all its children. Consumes 'term'.
void term_destroy(term_t *term);

// Evaluates given term (Reduction).
// Note: This function takes the ownership of term.
// Returns an owned pointer, or NULL if an error occurs.
term_t *term_eval(term_t *term);

// Gets a string representation of the term.
// Returns an owned pointer.
char *term_repr(const term_t *term);

// Decodes the given term into an integer.
// Returns TERM_INVALID_CHURCH if given term is not in Church Numeral format.
int term_as_church(const term_t *term);

// Encodes given number as a Church Numberal.
// Returns an owned pointer.
term_t *term_from_church(int num);

term_t *term_fuck(term_t *term);

#endif // TERM_H