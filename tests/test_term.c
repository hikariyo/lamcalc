#include "term.h"
#include "test.h"

TEST(eval_identity) {
    sym_init();
    sym_t x = sym_intern("x");
    sym_t a = sym_intern("a");

    term_t *id = term_new_abs(x, term_new_var(x));
    term_t *val = term_new_var(a);
    term_t *app = term_new_app(id, val);

    term_t *result = term_eval(app);

    TEST_ASSERT(result != NULL);
    TEST_ASSERT(result->type == TM_VAR);
    TEST_ASSERT(result->data.var == a);

    return PASSED;
}

TEST(eval_nested_app) {
    sym_init();
    sym_t x = sym_intern("x");
    sym_t y = sym_intern("y");
    sym_t z = sym_intern("z");
    sym_t a = sym_intern("a");

    // \z. z
    term_t *id = term_new_abs(z, term_new_var(z));
    // \x. \y. x y
    term_t *func = term_new_abs(
        x, term_new_abs(y, term_new_app(term_new_var(x), term_new_var(y))));

    // ((\x. \y. x y) (\z. z)) a
    term_t *app1 = term_new_app(func, id);
    term_t *app2 = term_new_app(app1, term_new_var(a));

    term_t *result = term_eval(app2);

    TEST_ASSERT(result->type == TM_VAR);
    TEST_ASSERT(result->data.var == a);

    term_destroy(result);
    return PASSED;
}

TEST(eval_higher_order) {
    sym_init();
    sym_t f = sym_intern("f");
    sym_t x = sym_intern("x");
    sym_t a = sym_intern("a");

    // \f. \x. f x
    term_t *apply = term_new_abs(
        f, term_new_abs(x, term_new_app(term_new_var(f), term_new_var(x))));
    // \z. z
    term_t *id = term_new_abs(sym_intern("z"), term_new_var(sym_intern("z")));

    term_t *app = term_new_app(term_new_app(apply, id), term_new_var(a));

    // (apply id) a
    term_t *res = term_eval(app);

    TEST_ASSERT(res->type == TM_VAR);
    TEST_ASSERT(res->data.var == a);

    term_destroy(res);
    return PASSED;
}

TEST(eval_free_var_preserved) {
    sym_init();
    sym_t x = sym_intern("x");
    sym_t y = sym_intern("y");
    sym_t a = sym_intern("a");

    // \x. x y
    term_t *body = term_new_app(term_new_var(x), term_new_var(y));
    term_t *abs = term_new_abs(x, body);

    term_t *app = term_new_app(abs, term_new_var(a));

    // (\x. x y) a
    term_t *res = term_eval(app);

    TEST_ASSERT(res->type == TM_APP);
    TEST_ASSERT(res->data.app.left->data.var == a);
    TEST_ASSERT(res->data.app.right->data.var == y);

    term_destroy(res);
    return PASSED;
}
