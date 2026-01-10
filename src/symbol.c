#include "symbol.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define SYM_DEFAULT_CAPACITY 32

typedef struct sym_table {
    char name[SYM_MAXLEN + 1]; // +1 for '\0'
} sym_table_t;

static sym_table_t *sym_table;
static size_t sym_count;
static size_t sym_capacity;

sym_t sym_intern(const char *name) {
    assert(strlen(name) <= SYM_MAXLEN);

    if (sym_capacity == 0) {
        // Initialize
        sym_capacity = SYM_DEFAULT_CAPACITY;
        sym_table = malloc(sym_capacity * sizeof(sym_table_t));
        assert(sym_table != NULL);
    }

    for (size_t i = 0; i < sym_count; i++) {
        if (!strcmp(sym_table[i].name, name)) {
            return i;
        }
    }

    if (sym_count + 1 > sym_capacity) {
        sym_capacity *= 2;
        sym_table = realloc(sym_table, sym_capacity * sizeof(sym_table_t));
        assert(sym_table != NULL);
    }

    strcpy(sym_table[sym_count].name, name);
    sym_count++;
    return sym_count - 1;
}

const char *sym_name(sym_t sym) {
    if (sym < 0 || sym > sym_count) {
        return SYM_INVALID;
    }
    return sym_table[sym].name;
}

void sym_destory() {
    free(sym_table);
    sym_count = 0;
    sym_capacity = 0;
    sym_table = NULL;
}
