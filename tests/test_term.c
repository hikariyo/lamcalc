#include "symbol.h"
#include "term.h"
#include "test.h"
#include <string.h>

TEST(term_eval_identity) {
    sym_t x = sym_intern("x");
    sym_t a = sym_intern("a");

    term_t *id = term_abs(x, term_var(x));
    term_t *val = term_var(a);
    term_t *app = term_app(id, val);

    term_t *result = term_eval(app);

    TEST_ASSERT(result != NULL);
    TEST_ASSERT(result->type == TM_VAR);
    TEST_ASSERT(result->data.var == a);

    term_destroy(result);

    return PASSED;
}

TEST(term_eval_nested_app) {
    sym_t x = sym_intern("x");
    sym_t y = sym_intern("y");
    sym_t z = sym_intern("z");
    sym_t a = sym_intern("a");

    // \z. z
    term_t *id = term_abs(z, term_var(z));
    // \x. \y. x y
    term_t *func = term_abs(x, term_abs(y, term_app(term_var(x), term_var(y))));

    // ((\x. \y. x y) (\z. z)) a
    term_t *app1 = term_app(func, id);
    term_t *app2 = term_app(app1, term_var(a));

    term_t *result = term_eval(app2);

    TEST_ASSERT(result->type == TM_VAR);
    TEST_ASSERT(result->data.var == a);

    term_destroy(result);
    return PASSED;
}

TEST(term_eval_higher_order) {
    sym_t f = sym_intern("f");
    sym_t x = sym_intern("x");
    sym_t a = sym_intern("a");

    // \f. \x. f x
    term_t *apply =
        term_abs(f, term_abs(x, term_app(term_var(f), term_var(x))));
    // \z. z
    term_t *id = term_abs(sym_intern("z"), term_var(sym_intern("z")));

    term_t *app = term_app(term_app(apply, id), term_var(a));

    // (apply id) a
    term_t *res = term_eval(app);

    TEST_ASSERT(res->type == TM_VAR);
    TEST_ASSERT(res->data.var == a);

    term_destroy(res);
    return PASSED;
}

TEST(term_eval_free_var_preserved) {
    sym_t x = sym_intern("x");
    sym_t y = sym_intern("y");
    sym_t a = sym_intern("a");

    // \x. x y
    term_t *body = term_app(term_var(x), term_var(y));
    term_t *abs = term_abs(x, body);

    term_t *app = term_app(abs, term_var(a));

    // (\x. x y) a
    term_t *res = term_eval(app);

    TEST_ASSERT(res->type == TM_APP);
    TEST_ASSERT(res->data.app.left->data.var == a);
    TEST_ASSERT(res->data.app.right->data.var == y);

    term_destroy(res);
    return PASSED;
}

TEST(term_eval_dummy_var_free) {
    // \x.\y.y
    sym_t x = sym_intern("x");
    sym_t y = sym_intern("y");
    sym_t a = sym_intern("a");

    term_t *abs = term_abs(x, term_abs(y, term_var(y)));
    term_t *app1 = term_app(abs, term_var(a));
    term_t *app2 = term_app(app1, term_var(a));
    term_t *result = term_eval(app2);

    TEST_ASSERT(result->type == TM_VAR);
    TEST_ASSERT(result->data.var == a);

    term_destroy(result);
    return PASSED;
}

TEST(term_repr) {
    sym_t x = sym_intern("x");
    sym_t y = sym_intern("y");
    sym_t z = sym_intern("z");
    sym_t a = sym_intern("a");

    term_t *id = term_abs(z, term_var(z));
    term_t *func = term_abs(x, term_abs(y, term_app(term_var(x), term_var(y))));

    term_t *app1 = term_app(func, id);
    term_t *app2 = term_app(app1, term_var(a));

    char *str = term_repr(app2);
    TEST_ASSERT(!strcmp("((\\x.\\y.(x y) \\z.z) a)", str));

    free(str);

    term_destroy(app2);
    return PASSED;
}
