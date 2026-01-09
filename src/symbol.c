#include "symbol.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define SYM_DEFAULT_CAPACITY 32

static char **sym_table;
static size_t sym_count;
static size_t sym_capacity;

sym_t sym_intern(const char *name) {
    for (size_t i = 0; i < sym_count; i++) {
        if (!strcmp(sym_table[i], name)) {
            return i;
        }
    }

    if (sym_count + 1 > sym_capacity) {
        sym_capacity *= 2;
        sym_table = realloc(sym_table, sym_capacity * sizeof(const char *));
        assert(sym_table != NULL);
    }

    sym_table[sym_count] = strdup(name);
    assert(sym_table[sym_count] != NULL);
    sym_count++;
    return sym_count - 1;
}

const char *sym_name(sym_t sym) {
    if (sym < 0 || sym > sym_count) {
        return "<INVALID>";
    }
    return sym_table[sym];
}

void sym_init() {
    for (size_t i = 0; i < sym_count; i++) {
        free(sym_table[i]);
    }
    free(sym_table);
    sym_count = 0;
    sym_capacity = SYM_DEFAULT_CAPACITY;
    sym_table = malloc(sym_capacity * sizeof(const char *));
    assert(sym_table != NULL);
}
