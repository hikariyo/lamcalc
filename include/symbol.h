#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdint.h>

#define SYM_INVALID "<INVALID>"
#define SYM_MAXLEN 63

typedef uint32_t sym_t;

void sym_destory();

sym_t sym_intern(const char *name);

const char *sym_name(sym_t sym);

#endif // SYMBOL_H