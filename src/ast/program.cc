#include "program.hh"
#include "../parser.hh"
#include "../gen.hh"

AST::Program::Program(location loc) : Node(loc) {
    scope = new AST::Scope(loc);
}

void AST::Program::write() {
    std::cout << "<Program>\n";
    scope->write();
    std::cout << "</Program>\n";
}

void AST::Program::push_globals(AST::Scope* s) {
    scope->merge_scope(s);
}

void AST::Program::push_function(Function* f) {
    scope->push_function(f);
}

void AST::Program::check_types(bool verbose) {
    for (auto i : scope->functions) {
        i->check_types(scope, verbose);
    }
}

std::string AST::Program::generate_ir() {
    std::string output = "; compiler build ";
    output += __DATE__;
    output += " ";
    output += __TIME__;
    output += "\n";

    /* 0. start codegen state */
    Gen::CodegenState gen;

    /* 1. traverse AST for function and constant IDs */
    for (auto i : scope->functions) {
        i->ic_reserve_ids(&gen);
    }

    /* add constants section */
    output += gen.get_constants_section();

    /* add globals section */
    output += gen.get_globals_section();

    /* add functions section */
    output += gen.get_functions_section();

    return output;
}
