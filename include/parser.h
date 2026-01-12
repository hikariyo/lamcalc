#ifndef PARSER_H
#define PARSER_H

struct term;
struct token;

// Syntax rules:
// Term    ::= AbsTerm | AppTerm
// AbsTerm ::= '\' NAME '.' Term
// AppTerm ::= Atom { Atom | AbsTerm }
// Atom    ::= NAME | LP Term RP ; VarTerm is single Atom

// Parses tokens into a term tree using recursive descent.
// Returns owned pointer, or NULL on syntax error; does not free original
// tokens.
struct term *parse(struct token **tokens, struct token *end);

// Parses given string.
// Returns owned pointer, or NULL on syntax error.
struct term *parse_string(const char *str);

#endif // PARSER_H