/*
 * driver.hh
 * declares the "driver" class which stores a parsing context
 */

#pragma once

#include <string>
#include <map>

#include "parser.hh"

/* define the correct yylex prototype for flex */
#define YY_DECL yy::parser::symbol_type yylex (driver& drv)
YY_DECL;

class driver {
public:
    driver();

    /* execute verbose scanner on filename f */
    int scan(const std::string& f);

    /* execute parser on filename f */
    int parse(const std::string& f);

    /* current parsing context */
    std::map<std::string, int> variables;
    int result;

    /* parsing config */
    std::string file;
    bool trace_parsing;

    /* encapsulate flex */
    void scan_begin();
    void scan_end();
    bool trace_scanning;
    yy::location location;
};
