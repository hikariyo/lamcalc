// clang-format off
#include "parser.h"
#include "term.h"
#include <stdio.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
// clang-format on

int main() {
    char *input = NULL;

    printf("Welcome to the REPL of lamcalc, a simple lambda calculus "
           "interpreter.\n");
    printf("Input \"exit\" without quotes to exit.\n");
    while ((input = readline("> ")) != NULL) {
        if (!strcmp(input, "exit")) {
            free(input);
            break;
        }

        if (*input == '\0') {
            goto next;
        }

        add_history(input);
        term_t *term = parse_string(input);
        if (term == NULL) {
            goto next;
        }

        term = term_eval(term);
        if (term == NULL) {
            goto next;
        }

        char *repr = term_repr(term);
        printf("%s\n", repr);
        free(repr);
        term_destroy(term);

    next:
        free(input);
    }
    return 0;
}
