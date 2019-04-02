#include "util.hh"

typedef yy::parser::token token;

std::string util::symbol_type_name(yy::parser::symbol_type& t) {
    /* boring function to get the name of a token/symbol */
    /* big 'switch' incoming */

    switch (t.token()) {
    case token::TOK_END:
        return "(EOF)";
    case token::TOK_TYPE:
        return "TYPE";
    case token::TOK_FOR:
        return "FOR";
    case token::TOK_WHILE:
        return "WHILE";
    case token::TOK_DO:
        return "DO";
    case token::TOK_IF:
        return "IF";
    case token::TOK_ELSE:
        return "ELSE";
    case token::TOK_BREAK:
        return "BREAK";
    case token::TOK_CONTINUE:
        return "CONTINUE";
    case token::TOK_RETURN:
        return "RETURN";
    case token::TOK_IDENT:
        return "IDENT";
    case token::TOK_INTCONST:
        return "INTCONST";
    case token::TOK_REALCONST:
        return "REALCONST";
    case token::TOK_STRCONST:
        return "STRCONST";
    case token::TOK_CHARCONST:
        return "CHARCONST";
    case token::TOK_LPAR:
        return "LPAR";
    case token::TOK_RPAR:
        return "RPAR";
    case token::TOK_LBRACKET:
        return "LBRACKET";
    case token::TOK_RBRACKET:
        return "RBRACKET";
    case token::TOK_LBRACE:
        return "LBRACE";
    case token::TOK_RBRACE:
        return "RBRACE";
    case token::TOK_COMMA:
        return "COMMA";
    case token::TOK_SEMI:
        return "SEMI";
    case token::TOK_QUEST:
        return "QUEST";
    case token::TOK_COLON:
        return "COLON";
    case token::TOK_EQUALS:
        return "EQUALS";
    case token::TOK_NEQUAL:
        return "NEQUAL";
    case token::TOK_GT:
        return "GT";
    case token::TOK_GE:
        return "GE";
    case token::TOK_LT:
        return "LT";
    case token::TOK_LE:
        return "LE";
    case token::TOK_PLUS:
        return "PLUS";
    case token::TOK_MINUS:
        return "MINUS";
    case token::TOK_STAR:
        return "STAR";
    case token::TOK_SLASH:
        return "SLASH";
    case token::TOK_MOD:
        return "MOD";
    case token::TOK_TILDE:
        return "TILDE";
    case token::TOK_PIPE:
        return "PIPE";
    case token::TOK_AMP:
        return "AMP";
    case token::TOK_BANG:
        return "BANG";
    case token::TOK_DPIPE:
        return "DPIPE";
    case token::TOK_DAMP:
        return "DAMP";
    case token::TOK_ASSIGN:
        return "ASSIGN";
    case token::TOK_PLUSASSIGN:
        return "PLUSASSIGN";
    case token::TOK_MINUSASSIGN:
        return "MINUSASSIGN";
    case token::TOK_STARASSIGN:
        return "STARASSIGN";
    case token::TOK_SLASHASSIGN:
        return "SLASHASSIGN";
    case token::TOK_INCR:
        return "INCR";
    case token::TOK_DECR:
        return "DECR";
    default:
        return "UNKNOWN";
    }
}
