#include "program.hh"
#include "../parser.hh"

AST::Program::Program(location& loc) : Node(loc) {}

void AST::Program::write() {
    std::cout << "<Program>\n";
    for (auto i : globals) i->write();
    std::cout << "</Program>\n";
}

void AST::Program::push_globals(std::vector<Variable*> vars) {
    for (auto i : vars) {
        for (auto j : globals) {
            if (i->name->name == j->name->name) {
                throw yy::parser::syntax_error(i->loc, "multiple definition of global variable " + i->name->name);
            }
        }

        globals.push_back(i);
    }
}

void AST::Program::push_function(Function* f) {
    /* we need to check for clashes with global vars,
     * as well as name clashes with functions. */
    for (auto i : globals) {
        if (i->name->name == f->name) {
            throw yy::parser::syntax_error(f->loc, f->name + " is already defined as a global variable");
        }
    }
}
