#ifndef PARSER_H
#define PARSER_H

struct term;
struct token;

// Syntax rules:
// Term    ::= AbsTerm | AppTerm
// AbsTerm ::= '\' NAME '.' Term
// AppTerm ::= Atom { Atom }
// Atom    ::= NAME | LP Term RP

// Parses tokens into a term tree using recursive descent.
// Returns owned pointer, or NULL on syntax error; does not free original
// tokens.
struct term *parse(struct token **tokens, struct token *end);

#endif // PARSER_H