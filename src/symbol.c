#include "symbol.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define SYM_DEFAULT_CAPACITY 32

typedef struct sym_table {
    char name[SYM_MAXLEN + 1]; // +1 for '\0'
} sym_table_t;

static sym_table_t *_table;
static size_t _count;
static size_t _capacity;

sym_t sym_intern(const char *name) {
    assert(strlen(name) <= SYM_MAXLEN);

    if (_capacity == 0) {
        // Initialize
        _capacity = SYM_DEFAULT_CAPACITY;
        _table = malloc(_capacity * sizeof(sym_table_t));
        assert(_table != NULL);
    }

    for (size_t i = 0; i < _count; i++) {
        if (!strcmp(_table[i].name, name)) {
            return i;
        }
    }

    if (_count + 1 > _capacity) {
        _capacity *= 2;
        _table = realloc(_table, _capacity * sizeof(sym_table_t));
        assert(_table != NULL);
    }

    strcpy(_table[_count].name, name);
    _count++;
    return _count - 1;
}

const char *sym_name(sym_t sym) {
    if (sym < 0 || sym > _count) {
        return SYM_INVALID;
    }
    return _table[sym].name;
}

void sym_destroy() {
    free(_table);
    _count = 0;
    _capacity = 0;
    _table = NULL;
}
