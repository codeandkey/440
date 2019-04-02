/*
 * scanner.ll
 * scanner flex input file
 */

%{
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <string>
#include "driver.hh"
#include "parser.hh"
%}

%option noyywrap nounput noinput batch debug

/* helper function to make a number symbol with the appropriate value */
%{
    yy::parser::symbol_type make_NUMBER (const std::string&s, const yy::parser::location_type& loc);
%}

id    [a-zA-Z][a-zA-Z0-9]*
int   [0-9]+
blank [ \t]

/* executed on every pattern match */
%{
    #define YY_USER_ACTION loc.columns(yyleng);
%}

%%

%{
    /* shortcut for token matching, run on every yylex() */
    yy::location& loc = drv.location;
    loc.step();
%}

{blank}+  loc.step();
\n+       loc.lines(yyleng); loc.step();

"-"   return yy::parser::make_MINUS(loc);
"+"   return yy::parser::make_PLUS(loc);
"="   return yy::parser::make_ASSIGN(loc);
"("   return yy::parser::make_LPAREN(loc);
")"   return yy::parser::make_RPAREN(loc);

{int} return make_NUMBER(yytext, loc);
{id}  return yy::parser::make_IDENTIFIER(yytext, loc);

. {
    throw yy::parser::syntax_error(loc, "invalid character: " + std::string(yytext));
}

<<EOF>> return yy::parser::make_END(loc);

%%

yy::parser::symbol_type make_NUMBER(const std::string& s, const yy::parser::location_type& loc) {
    errno = 0;
    long n = strtol(s.c_str(), NULL, 10);
    if (errno) {
        throw yy::parser::syntax_error(loc, "invalid number " + s + ": " + strerror(errno));
    }
    return yy::parser::make_NUMBER((int) n, loc);
}

void driver::scan_begin() {
    yy_flex_debug = trace_scanning;
    if (file.empty() || file == "-") {
        yyin = stdin;
    } else if (!(yyin = fopen(file.c_str(), "r"))) {
        std::cerr << "cannot open " << file << ": " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }
}

void driver::scan_end() {
    fclose(yyin);
}
