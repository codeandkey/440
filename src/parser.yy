/*
 * parser.yy
 * bison parser input file
 */

%skeleton "lalr1.cc" /* use C++ skeleton */
%require "3.2"       /* require bison 3.2+ */
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

/* forward-declare parse driver */
%code requires {
    #include <string>
    class driver;
}

%param {
    driver& drv
}

/* request location tracking */
%locations

/* enable parser tracing, verbose error messages */
%define parse.trace
%define parse.error verbose

%code {
    #include "driver.hh"
}

/* token definitions, nonterminals */
%define api.token.prefix {TOK_}
%token
    END 0       "end of file"
    FOR         "for"
    WHILE       "while"
    DO          "do"
    IF          "if"
    ELSE        "else"
    BREAK       "break"
    CONTINUE    "continue"
    RETURN      "return"
    LPAR        "("
    RPAR        ")"
    LBRACKET    "["
    RBRACKET    "]"
    LBRACE      "{"
    RBRACE      "}"
    COMMA       ","
    SEMI        ";"
    QUEST       "?"
    COLON       ":"
    EQUALS      "=="
    NEQUAL      "!="
    GT          ">"
    GE          ">="
    LT          "<"
    LE          "<="
    PLUS        "+"
    MINUS       "-"
    STAR        "*"
    SLASH       "/"
    MOD         "%"
    TILDE       "~"
    PIPE        "|"
    AMP         "&"
    BANG        "!"
    DPIPE       "||"
    DAMP        "&&"
    ASSIGN      "="
    PLUSASSIGN  "+="
    MINUSASSIGN "-="
    STARASSIGN  "*="
    SLASHASSIGN "/="
    INCR        "++"
    DECR        "--"
;

%token <std::string> TYPE       "type"
%token <std::string> IDENT      "identifier"
%token <int>         INTCONST   "integer constant"
%token <double>      REALCONST  "real constant"
%token <std::string> STRCONST   "string literarl"
%token <char>        CHARCONST  "character constant"

/* define semantic value printing */
%printer { yyo << $$; } <*>;

%% /* -- GRAMMAR DEFINITION -- */
%start unit;

unit: %empty

%%

void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << l << ": " << m << "\n";
}
