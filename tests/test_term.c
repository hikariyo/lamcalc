#include "symbol.h"
#include "term.h"
#include "test.h"
#include <string.h>

TEST(term_eval_identity) {
    sym_t x = sym_intern("x");
    sym_t a = sym_intern("a");

    term_t *id = term_abs(x, term_var(x, 0));
    term_t *val = term_var(a, TERM_VAR_INVALID_INDEX);
    term_t *app = term_app(id, val);

    term_t *result = term_eval(app);

    TEST_ASSERT(result != NULL);
    TEST_ASSERT(result->type == TM_VAR);
    TEST_ASSERT(result->data.var.sym == a);

    term_destroy(result);

    return PASSED;
}

TEST(term_eval_nested_app) {
    sym_t x = sym_intern("x");
    sym_t y = sym_intern("y");
    sym_t z = sym_intern("z");
    sym_t a = sym_intern("a");

    // \z. z
    term_t *id = term_abs(z, term_var(z, 0));
    // \x. \y. x y
    term_t *func =
        term_abs(x, term_abs(y, term_app(term_var(x, 1), term_var(y, 0))));

    // ((\x. \y. x y) (\z. z)) a
    term_t *app1 = term_app(func, id);
    term_t *app2 = term_app(app1, term_var(a, TERM_VAR_INVALID_INDEX));

    term_t *result = term_eval(app2);

    TEST_ASSERT(result->type == TM_VAR);
    TEST_ASSERT(result->data.var.sym == a);

    term_destroy(result);
    return PASSED;
}

TEST(term_eval_higher_order) {
    sym_t f = sym_intern("f");
    sym_t x = sym_intern("x");
    sym_t a = sym_intern("a");

    // \f. \x. f x
    term_t *apply =
        term_abs(f, term_abs(x, term_app(term_var(f, 1), term_var(x, 0))));
    // \z. z
    term_t *id = term_abs(sym_intern("z"), term_var(sym_intern("z"), 0));

    term_t *app =
        term_app(term_app(apply, id), term_var(a, TERM_VAR_INVALID_INDEX));

    // (apply id) a
    term_t *res = term_eval(app);

    TEST_ASSERT(res->type == TM_VAR);
    TEST_ASSERT(res->data.var.sym == a);

    term_destroy(res);
    return PASSED;
}
