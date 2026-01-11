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

void print_execution_time(struct timespec start, struct timespec end) {
    // 计算总纳秒差
    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    double total_ns = (double)seconds * 1e9 + (double)nanoseconds;

    printf(" (");
    if (total_ns < 1000) {
        printf("%.0f ns", total_ns);
    } else if (total_ns < 1000000) {
        printf("%.2f μs", total_ns / 1000.0);
    } else {
        printf("%.2f ms", total_ns / 1000000.0);
    }
    printf(")");
}

int main() {
    char *input = NULL;

    printf("Welcome to the REPL of lamcalc, a simple lambda calculus "
           "interpreter.\n");
    printf("Input \"exit\" without quotes to exit.\n");
    struct timespec start, end;
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
        if (term == NULL) {
            goto next;
        }

        term = term_eval(term);
        if (term == NULL) {
            goto next;
        }

        char *repr = term_repr(term);
        printf("%s", repr);
        free(repr);
        term_destroy(term);
        clock_gettime(CLOCK_MONOTONIC, &end);
        print_execution_time(start, end);
        printf("\n");
        fflush(stdout);
    next:
        free(input);
    }
    return 0;
}
