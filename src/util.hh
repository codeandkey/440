#pragma once

/*
 * utility functions
 */

#include "parser.hh"

namespace util {
    std::string symbol_type_name(yy::parser::symbol_type& t);
}
