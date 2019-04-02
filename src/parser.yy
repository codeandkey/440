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
    END 0  "end of file"
    ASSIGN "="
    MINUS  "-"
    PLUS   "+"
    LPAREN "("
    RPAREN ")"
;

%token <std::string> IDENTIFIER "identifier"
%token <int>         NUMBER     "number"
%type  <int>         exp

/* define semantic value printing */
%printer { yyo << $$; } <*>;

%% /* -- GRAMMAR DEFINITION -- */
%start unit;

unit: assignments exp { drv.result = $2; };

assignments:
    %empty                 {}
  | assignments assignment {};

assignment:
    "identifier" "=" exp { drv.variables[$1] = $3; };

%left "+" "-";

exp:
    "number"
  | "identifier"  { $$ = drv.variables[$1]; }
  | exp "+" exp   { $$ = $1 + $3; }
  | exp "-" exp   { $$ = $1 - $3; }
  | "(" exp ")"   { $$ = $2; }
%%

void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << l << ": " << m << "\n";
}
