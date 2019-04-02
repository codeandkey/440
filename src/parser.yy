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
    #include "ast.hh"
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
%token <std::string> STRCONST   "string literal"
%token <char>        CHARCONST  "character constant"

%type <AST::Program*>                   program              "program"
%type <AST::Expression*>                expression           "expression"
%type <std::vector<AST::Variable*>>     variable_declaration "variable declaration"
%type <std::vector<AST::VariableName*>> variable_names       "variable names"
%type <AST::VariableName*>              variable_name        "variable name"

%% /* -- GRAMMAR DEFINITION -- */
%start unit;

unit: program { $1->write(); }

program:
    %empty                         { $$ = new AST::Program(@$); }
    | program variable_declaration { $$ = $1; $$->push_globals($2); }
    ;

variable_declaration:
    TYPE variable_names SEMI { for (auto v : $2) $$.push_back(new AST::Variable(@$, $1, v)); }
    ;

variable_names:
    variable_name                        { $$.push_back($1); }
    | variable_names COMMA variable_name { $$ = $1; $$.push_back($3); } 
    ;

variable_name:
    IDENT                              { $$ = new AST::VariableName(@$, $1); }
    | IDENT LBRACKET INTCONST RBRACKET { $$ = new AST::VariableName(@$, $1, $3); }
    ;

expression: INTCONST { $$ = new AST::IntConst($1); }

%%

void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << "Error in " << *(l.begin.filename) << " line " << l.begin.line << "\n\t" << m << "\n";
}
