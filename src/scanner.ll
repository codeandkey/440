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

/* helper functions to construct attributed tokens */
%{
    yy::parser::symbol_type make_STRCONST  (const std::string&s, const yy::parser::location_type& loc);
    yy::parser::symbol_type make_CHARCONST (const std::string&s, const yy::parser::location_type& loc);
%}

id        [a-zA-Z][a-zA-Z0-9]*
intconst  -?[0-9]+
realconst -?(([0-9]+(\.[0-9]+)?(e[0-9]+)?)|(\.[0-9]+))f?
strconst  \"(\\.|[^\\\"])*\" 
charconst \'\\?.\'
blank     [ \t]
type      (void|char|int|float)

/* executed on every pattern match */
%{
    #define YY_USER_ACTION loc.columns(yyleng);
%}

/* state for consuming long comments */
%x LONG_COMMENT

%%

%{
    /* shortcut for token matching, run on every yylex() */
    yy::location& loc = drv.location;
    loc.step();
%}

{blank}+  loc.step();
\n+       loc.lines(yyleng); loc.step();

\/\/.*\n           {}
"/*"               { BEGIN(LONG_COMMENT); }
<LONG_COMMENT>"*/" { BEGIN(INITIAL); }
<LONG_COMMENT>\n   {}
<LONG_COMMENT>.    {}

"for"      return yy::parser::make_FOR(loc);
"while"    return yy::parser::make_WHILE(loc);
"do"       return yy::parser::make_DO(loc);
"if"       return yy::parser::make_IF(loc);
"else"     return yy::parser::make_ELSE(loc);
"break"    return yy::parser::make_BREAK(loc);
"continue" return yy::parser::make_CONTINUE(loc);
"return"   return yy::parser::make_RETURN(loc);
"("        return yy::parser::make_LPAR(loc);
")"        return yy::parser::make_RPAR(loc);
"["        return yy::parser::make_LBRACKET(loc);
"]"        return yy::parser::make_RBRACKET(loc);
"{"        return yy::parser::make_LBRACE(loc);
"}"        return yy::parser::make_RBRACE(loc);
","        return yy::parser::make_COMMA(loc);
";"        return yy::parser::make_SEMI(loc);
"?"        return yy::parser::make_QUEST(loc);
":"        return yy::parser::make_COLON(loc);
"=="       return yy::parser::make_EQUALS(loc);
"!="       return yy::parser::make_NEQUAL(loc);
">"        return yy::parser::make_GT(loc);
">="       return yy::parser::make_GE(loc);
"<"        return yy::parser::make_LT(loc);
"<="       return yy::parser::make_LE(loc);
"+"        return yy::parser::make_PLUS(loc);
"-"        return yy::parser::make_MINUS(loc);
"*"        return yy::parser::make_STAR(loc);
"/"        return yy::parser::make_SLASH(loc);
"%"        return yy::parser::make_MOD(loc);
"~"        return yy::parser::make_TILDE(loc);
"|"        return yy::parser::make_PIPE(loc);
"&"        return yy::parser::make_AMP(loc);
"!"        return yy::parser::make_BANG(loc);
"||"       return yy::parser::make_DPIPE(loc);
"&&"       return yy::parser::make_DAMP(loc);
"="        return yy::parser::make_ASSIGN(loc);
"+="       return yy::parser::make_PLUSASSIGN(loc);
"-="       return yy::parser::make_MINUSASSIGN(loc);
"*="       return yy::parser::make_STARASSIGN(loc);
"/="       return yy::parser::make_SLASHASSIGN(loc);
"++"       return yy::parser::make_INCR(loc);
"--"       return yy::parser::make_DECR(loc);

{type}     return yy::parser::make_TYPE(yytext, loc);
{id}       return yy::parser::make_IDENT(yytext, loc);

{intconst}  return yy::parser::make_INTCONST(atoi(yytext), loc);
{realconst} return yy::parser::make_REALCONST(atof(yytext), loc);
{strconst}  return make_STRCONST(yytext, loc);
{charconst} return make_CHARCONST(yytext, loc);

. {
    throw yy::parser::syntax_error(loc, "invalid character: " + std::string(yytext));
}

<<EOF>> return yy::parser::make_END(loc);

%%

/*
 * make_STRCONST(s, loc)
 * chop off the '"' characters at the beginning and end of a string literal lexeme
 */
yy::parser::symbol_type make_STRCONST(const std::string& s, const yy::parser::location_type& loc) {
    return yy::parser::make_STRCONST(s.substr(1, s.size() - 2), loc);
}

/*
 * make_CHARCONST(s, loc)
 * grab the character from a character constant lexeme
 */
yy::parser::symbol_type make_CHARCONST(const std::string& s, const yy::parser::location_type& loc) {
    /* for escaped strings, return valid sequences or fail */
    if (s.size() == 4) {
        switch (s[2]) {
        case 'n':
            return yy::parser::make_CHARCONST('\n', loc);
        case 't':
            return yy::parser::make_CHARCONST('\t', loc);
        case 'r':
            return yy::parser::make_CHARCONST('\r', loc);
        default:
            throw yy::parser::syntax_error(loc, "unrecognized character escape sequence: " + s);
        }
    }

    /* non escape character, pass through the second character in the lexeme */
    return yy::parser::make_CHARCONST(s[1], loc);
}

void driver::scan_begin() {
    yy_flex_debug = trace_scanning;
    if (file.empty() || file == "-") {
        yyin = stdin;
    } else if (!(yyin = fopen(file.c_str(), "r"))) {
        std::cerr << "error: cannot open " << file << ": " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }
}

void driver::scan_end() {
    fclose(yyin);
}
