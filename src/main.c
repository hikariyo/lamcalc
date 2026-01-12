// clang-format off
#include "parser.h"
#include "term.h"
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// clang-format on

void print_execution_time(struct timespec start, struct timespec endparse,
                          struct timespec endeval, struct timespec endrepr) {
    double p = (double)(endparse.tv_sec - start.tv_sec) * 1e3 +
               (double)(endparse.tv_nsec - start.tv_nsec) / 1e6;

    double e = (double)(endeval.tv_sec - endparse.tv_sec) * 1e3 +
               (double)(endeval.tv_nsec - endparse.tv_nsec) / 1e6;

    double r = (double)(endrepr.tv_sec - endeval.tv_sec) * 1e3 +
               (double)(endrepr.tv_nsec - endeval.tv_nsec) / 1e6;

    double total = p + e + r;
    const char *DIM = "\033[2m";
    const char *RESET = "\033[0m";
    printf("%s[ Done in %.3fms | p: %.3f, e: %.3f, r: %.3f ]%s\n", DIM, total,
           p, e, r, RESET);
}

int main() {
    char *input = NULL;

    printf("Welcome to the REPL of lamcalc, a simple lambda calculus "
           "interpreter.\n");
    printf("Input \"exit\" without quotes to exit. (p: parse, e: eval, r: "
           "repr)\n");
    struct timespec start, endparse, endeval, endrepr;
    while ((input = readline("> ")) != NULL) {
        if (!strcmp(input, "exit")) {
            free(input);
            break;
        }

        if (*input == '\0') {
            goto next;
        }

        add_history(input);

        clock_gettime(CLOCK_MONOTONIC, &start);
        term_t *term = parse_string(input);
        clock_gettime(CLOCK_MONOTONIC, &endparse);

        if (term == NULL) {
            goto next;
        }

        term = term_eval(term);
        clock_gettime(CLOCK_MONOTONIC, &endeval);

        if (term == NULL) {
            goto next;
        }

        char *repr = term_repr(term);
        clock_gettime(CLOCK_MONOTONIC, &endrepr);

        printf("%s\n", repr);
        free(repr);
        term_destroy(term);
        print_execution_time(start, endparse, endeval, endrepr);

        fflush(stdout);
    next:
        free(input);
    }
    return 0;
}
