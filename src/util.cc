#include "util.hh"

typedef yy::parser::token token;

std::string util::symbol_type_name(yy::parser::symbol_type& t) {
    /* boring function to get the name of a token/symbol */
    /* big 'switch' incoming */

    switch (t.token()) {
    case token::TOK_END:
        return "(EOF)";
    case token::TOK_ASSIGN:
        return "ASSIGN";
    case token::TOK_MINUS:
        return "MINUS";
    case token::TOK_PLUS:
        return "PLUS";
    case token::TOK_LPAREN:
        return "LPAREN";
    case token::TOK_RPAREN:
        return "RPAREN";
    case token::TOK_IDENTIFIER:
        return "IDENT";
    case token::TOK_NUMBER:
        return "NUMBER";
    default:
        return "NONAME";
    }
}
