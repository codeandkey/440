#include "program.hh"
#include "../parser.hh"

AST::Program::Program(location loc) : Node(loc) {}

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

        for (auto j : functions) {
            if (i->name->name == j->name) {
                throw yy::parser::syntax_error(i->loc, i->name->name + " already defined as a function at " + *(i->loc.begin.filename) + ":" + std::to_string(i->loc.begin.line));
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
            throw yy::parser::syntax_error(f->loc, f->name + " already defined as a global variable at " + *(i->loc.begin.filename) + ":" + std::to_string(i->loc.begin.line));
        }
    }

    /* then, check if it was declared already.
     * if it is already declared make sure everything matches */
    for (auto i : functions) {
        if (i->name == f->name) {
            if (i->params.size() != f->params.size()) {
                throw yy::parser::syntax_error(f->loc, f->name + " already declared with " + std::to_string(i->params.size()) + " arguments at " + *(i->loc.begin.filename) + ":" + std::to_string(i->loc.begin.line));
            }

            for (unsigned long p = 0; p < i->params.size(); ++p) {
                if (i->params[p]->base_type != f->params[p]->base_type || i->params[p]->name->is_array != f->params[p]->name->is_array) {
                    std::string orig_type = i->params[p]->base_type + (i->params[p]->name->is_array ? "[]" : "");
                    throw yy::parser::syntax_error(f->loc, f->name + " declared with different type " + orig_type + " for parameter " + std::to_string(p+1) + " at " + *(i->loc.begin.filename) + ":" + std::to_string(i->loc.begin.line));
                }
            }

            if (i->ret_type != f->ret_type) {
                throw yy::parser::syntax_error(f->loc, f->name + " was already declared with return type " + i->ret_type + " at " + *(i->loc.begin.filename) + ":" + std::to_string(i->loc.begin.line));
            }

            /* declarations match up. throw an error if we're trying to redefine the function */
            if (i->defined && f->defined) {
                throw yy::parser::syntax_error(f->loc, "multiple definition of " + f->name + "; previously defined at " + *(i->loc.begin.filename) + ":" + std::to_string(i->loc.begin.line));
            }

            /* if we define the function, set the location */
            if (f->defined) {
                i->locals = f->locals;
                i->body = f->body;
                i->loc = f->loc;
                i->defined = true;
                return;
            }
        }
    }

    /* function not defined yet. we can just push it to the list regardless */
    functions.push_back(f);
}
