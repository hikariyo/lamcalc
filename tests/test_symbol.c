#include "symbol.h"
#include "test.h"
#include <string.h>

TEST(symbol) {
    const char *sym1 = "symbol 1";
    const char *sym2 = "symbol 2";

    sym_t s1 = sym_intern(sym1);
    sym_t s2 = sym_intern(sym2);

    TEST_ASSERT(!strcmp(sym_name(s1), sym1));
    TEST_ASSERT(!strcmp(sym_name(s2), sym2));
    TEST_ASSERT(!strcmp(sym_name(998244353), SYM_INVALID));

    return PASSED;
}
