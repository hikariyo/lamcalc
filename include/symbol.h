#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdint.h>

#define SYM_INVALID "<INVALID>"
#define SYM_MAXLEN 63

typedef uint32_t sym_t;

// Destroys the global symbol table and frees all internal memory.
void sym_destroy();

// Interns the given name into the global table.
// Returns a unique symbol ID.
// If the name already exists, returns the existing symbol.
sym_t sym_intern(const char *name);

// Gets the string name associated with given symbol.
// Returns SYM_INVALID if the symbol is not found in the table.
// Note: The returned pointer is managed by the symbol table.
const char *sym_name(sym_t sym);

#endif // SYMBOL_H