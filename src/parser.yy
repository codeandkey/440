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

/* semantic tokens */
%token <std::string> TYPE       "type"
%token <std::string> IDENT      "identifier"
%token <int>         INTCONST   "integer constant"
%token <double>      REALCONST  "real constant"
%token <std::string> STRCONST   "string literal"
%token <char>        CHARCONST  "character constant"

/* nonterminal symbols */
%type <AST::Program*>                   program              "program"
%type <AST::Expression*>                expression           "expression"
%type <AST::Scope*>                     variable_declaration "variable declaration"
%type <AST::Scope*>                     parameter_list       "parameter list"
%type <std::vector<AST::VariableName*>> variable_names       "variable names"
%type <AST::VariableName*>              variable_name        "variable name"
%type <AST::Variable*>                  formal_param         "formal parameter"
%type <AST::Function*>                  function_prototype   "function prototype"
%type <AST::Function*>                  function_definition  "function definition"
%type <AST::Scope*>                     function_locals      "function locals"
%type <std::vector<AST::Statement*>>    function_body        "function body"
%type <AST::Statement*>                 statement            "statement"

%% /* -- GRAMMAR DEFINITION -- */
%start unit;

unit: program { drv.result = $1; }

program:
    %empty                         { $$ = new AST::Program(@$); }
    | program variable_declaration { $$ = $1; $$->push_globals($2); }
    | program function_prototype   { $$ = $1; $$->push_function($2); }
    | program function_definition  { $$ = $1; $$->push_function($2); }
    ;

variable_declaration:
    TYPE variable_names SEMI { $$ = new AST::Scope(@$, $1, $2); }
    ;

variable_names:
    variable_name                        { $$.push_back($1); }
    | variable_names COMMA variable_name { $$ = $1; $$.push_back($3); } 
    ;

variable_name:
    IDENT                              { $$ = new AST::VariableName(@1, $1); }
    | IDENT LBRACKET INTCONST RBRACKET { $$ = new AST::VariableName(@1, $1, $3); }
    ;

formal_param:
    TYPE variable_name { $$ = new AST::Variable(@$, $1, $2); }
    ;

parameter_list:
    %empty                              { $$ = new AST::Scope(@$); }
    | formal_param                      { $$ = new AST::Scope(@1, $1); }
    | parameter_list COMMA formal_param { $$ = $1; $$->push_variable($3); }
    ;

function_locals:
    %empty                                 {}
    | variable_declaration                 { $$ = $1; }
    | function_locals variable_declaration { $$ = new AST::Scope(@$, $1, $2); }
    ;

function_body:
    %empty                    {}
    | statement               { $$.push_back($1); }
    | function_body statement { $$ = $1; $$.push_back($2); }
    ;

function_prototype:
    TYPE IDENT LPAR parameter_list RPAR SEMI { $$ = new AST::Function(@2, $1, $2, $4); }
    ;

function_definition:
    TYPE IDENT LPAR parameter_list RPAR LBRACE function_locals function_body RBRACE { $$ = new AST::Function(@2, $1, $2, $4, $7, $8); }
    ;

statement:
    expression SEMI  { $$ = new AST::ExpressionStatement(@1, $1); }
    ;

expression: INTCONST { $$ = new AST::IntConst(@1, $1); }

%%

void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << "Error in " << *(l.begin.filename) << " line " << l.begin.line << "\n\t" << m << "\n";
}
