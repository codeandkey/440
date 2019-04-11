#include "program.hh"
#include "../parser.hh"

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
