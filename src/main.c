#include "lexer.h"
#include "parser.h"
#include "term.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

        if (*input != '\0') {
            add_history(input);
            token_t *tokens = lex_string(input);
            token_t *cursor = tokens;

            term_t *term = parse(&cursor, NULL);
            if (term != NULL) {
                term = term_eval(term);
                if (term != NULL) {
                    char *repr = term_repr(term);
                    printf("%s\n", repr);
                    free(repr);
                    term_destroy(term);
                }
            }

            lex_destroy_tokens(tokens);
        }
        free(input);
    }
    return 0;
}
