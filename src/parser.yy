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
%type <AST::Expression*>                optional_expression  "optional expression"
%type <AST::Scope*>                     variable_declaration "variable declaration"
%type <AST::Scope*>                     parameter_list       "parameter list"
%type <std::vector<AST::VariableName*>> variable_names       "variable names"
%type <AST::VariableName*>              variable_name        "variable name"
%type <AST::VariableName*>              parameter_name       "parameter name"
%type <AST::Variable*>                  formal_param         "formal parameter"
%type <AST::Function*>                  function_prototype   "function prototype"
%type <AST::Function*>                  function_definition  "function definition"
%type <AST::Scope*>                     function_locals      "function locals"
%type <std::vector<AST::Statement*>>    function_body        "function body"
%type <AST::Statement*>                 statement            "statement"
%type <std::vector<AST::Statement*>>    control_body         "control body"
%type <std::vector<AST::Statement*>>    statement_block      "statement block"
%type <std::vector<AST::Statement*>>    statement_list       "statement list"
%type <AST::AssignmentExpression::Type> assignment_op        "assignment operator"
%type <AST::UnaryOpExpression::Type>    unary_op             "unary operator"
%type <AST::BinaryOpExpression::Type>   binary_op            "binary operator"
%type <AST::IncDecExpression::Type>     inc_dec_op           "increment/decrement operator"
%type <AST::LValue*>                    l_value              "lvalue"
%type <std::vector<AST::Expression*>>   argument_list        "argument list"

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

parameter_name:
    IDENT                     { $$ = new AST::VariableName(@1, $1); }
    | IDENT LBRACKET RBRACKET { $$ = new AST::VariableName(@1, $1, 0); }
    ;

formal_param:
    TYPE parameter_name { $$ = new AST::Variable(@$, $1, $2); }
    ;

parameter_list:
    %empty                              { $$ = new AST::Scope(@$); }
    | formal_param                      { $$ = new AST::Scope(@1, $1); }
    | parameter_list COMMA formal_param { $$ = $1; $$->push_variable($3); }
    ;

function_locals:
    %empty                                 { $$ = new AST::Scope(@$); }
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

control_body:
    statement       { $$.push_back($1); }
    | statement_block { $$ = $1; }
    ;

statement_block:
    LBRACE statement_list RBRACE { $$ = $2; }
    ;

statement_list:
    %empty                     {}
    | statement                { $$.push_back($1); }
    | statement_list statement { $$ = $1; $$.push_back($2); }
    ;

statement:
    expression SEMI                      { $$ = new AST::ExpressionStatement(@1, $1); }
    | BREAK SEMI                           { $$ = new AST::BreakStatement(@1); }
    | CONTINUE SEMI                        { $$ = new AST::ContinueStatement(@1); }
    | RETURN optional_expression SEMI      { $$ = new AST::ReturnStatement(@1, $2); }
    | IF LPAR expression RPAR control_body                   { $$ = new AST::IfStatement(@1, $3, $5); }
    | IF LPAR expression RPAR control_body ELSE control_body { $$ = new AST::IfStatement(@1, $3, $5, $7); }
    | FOR LPAR optional_expression SEMI optional_expression SEMI optional_expression RPAR control_body { $$ = new AST::ForStatement(@1, $3, $5, $7, $9); }
    | WHILE LPAR expression RPAR control_body { $$ = new AST::WhileStatement(@1, $3, $5); }
    | DO control_body WHILE LPAR expression RPAR SEMI { $$ = new AST::DoWhileStatement(@1, $5, $2); }
    ;

optional_expression:
    %empty       { $$ = NULL; }
    | expression { $$ = $1; }
    ;

inc_dec_op:
    INCR   { $$ = AST::IncDecExpression::Type::INCR; }
    | DECR { $$ = AST::IncDecExpression::Type::DECR; }
    ;

unary_op:
    MINUS   { $$ = AST::UnaryOpExpression::Type::MINUS; }
    | BANG  { $$ = AST::UnaryOpExpression::Type::BANG; }
    | TILDE { $$ = AST::UnaryOpExpression::Type::TILDE; }
    ;

binary_op:
    EQUALS   { $$ = AST::BinaryOpExpression::Type::EQUALS; }
    | NEQUAL { $$ = AST::BinaryOpExpression::Type::NEQUAL; }
    | GT     { $$ = AST::BinaryOpExpression::Type::GT; }
    | GE     { $$ = AST::BinaryOpExpression::Type::GE; }
    | LT     { $$ = AST::BinaryOpExpression::Type::LT; }
    | LE     { $$ = AST::BinaryOpExpression::Type::LE; }
    | PLUS   { $$ = AST::BinaryOpExpression::Type::PLUS; }
    | MINUS  { $$ = AST::BinaryOpExpression::Type::MINUS; }
    | STAR   { $$ = AST::BinaryOpExpression::Type::STAR; }
    | SLASH  { $$ = AST::BinaryOpExpression::Type::SLASH; }
    | MOD    { $$ = AST::BinaryOpExpression::Type::MOD; }
    | PIPE   { $$ = AST::BinaryOpExpression::Type::PIPE; }
    | AMP    { $$ = AST::BinaryOpExpression::Type::AMP; }
    | DPIPE  { $$ = AST::BinaryOpExpression::Type::DPIPE; }
    | DAMP   { $$ = AST::BinaryOpExpression::Type::DAMP; }
    ;

assignment_op:
    ASSIGN        { $$ = AST::AssignmentExpression::Type::ASSIGN; }
    | PLUSASSIGN  { $$ = AST::AssignmentExpression::Type::PLUSASSIGN; }
    | MINUSASSIGN { $$ = AST::AssignmentExpression::Type::MINUSASSIGN; }
    | STARASSIGN  { $$ = AST::AssignmentExpression::Type::STARASSIGN; }
    | SLASHASSIGN { $$ = AST::AssignmentExpression::Type::SLASHASSIGN; }
    ;

l_value:
    IDENT                                { $$ = new AST::LValue(@1, $1, NULL); }
    | IDENT LBRACKET expression RBRACKET { $$ = new AST::LValue(@1, $1, $3); }
    ;

argument_list:
    %empty                           {}
    | expression                     { $$.push_back($1); }
    | argument_list COMMA expression { $$ = $1; $$.push_back($3); }
    ;

expression:
    INTCONST                                       { $$ = new AST::IntConst(@1, $1); }
    | REALCONST                                    { $$ = new AST::RealConst(@1, $1); }
    | STRCONST                                     { $$ = new AST::StrConst(@1, $1); }
    | CHARCONST                                    { $$ = new AST::CharConst(@1, $1); }
    | IDENT                                        { $$ = new AST::IdentifierExpression(@1, $1); }
    | IDENT LBRACKET expression RBRACKET           { $$ = new AST::IndexExpression(@$, $1, $3); }
    | IDENT LPAR argument_list RPAR                { $$ = new AST::CallExpression(@$, $1, $3); }
    | l_value assignment_op expression             { $$ = new AST::AssignmentExpression(@$, $1, $2, $3); }
    | inc_dec_op l_value                           { $$ = new AST::IncDecExpression(@$, $2, $1, true); }
    | l_value inc_dec_op                           { $$ = new AST::IncDecExpression(@$, $1, $2, false); }
    | unary_op expression                          { $$ = new AST::UnaryOpExpression(@$, $2, $1); }
    | expression binary_op expression              { $$ = new AST::BinaryOpExpression(@$, $1, $3, $2); }
    | expression QUEST expression COLON expression { $$ = new AST::TernaryOpExpression(@$, $1, $3, $5); }
    | LPAR TYPE RPAR expression                    { $$ = new AST::CastExpression(@$, $2, $4); }
    | LPAR expression RPAR                         { $$ = $2; }
    ;

%%

void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << "Error in " << *(l.begin.filename) << " line " << l.begin.line << "\n\t" << m << "\n";
}
