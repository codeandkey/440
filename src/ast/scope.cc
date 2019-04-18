#include "scope.hh"
#include "../parser.hh"

AST::Scope::Scope(location loc) : Node(loc) {}

AST::Scope::Scope(location loc, AST::Scope* a, AST::Scope* b) : Node(loc) {
    /* don't mutate during merge */

    variables = a->variables;
    functions = a->functions;

    merge_scope(b);
}

AST::Scope::Scope(location loc, std::string type, std::vector<AST::VariableName*> names) : Node(loc) {
    for (auto i : names) {
        push_variable(new AST::Variable(loc, type, i));
    }
}

AST::Scope::Scope(location loc, AST::Variable* first) : Node(loc) {
    push_variable(first);
}

AST::Scope::~Scope() {}

void AST::Scope::push_variable(AST::Variable* v) {
    for (auto i : variables) {
        if (i->name->name == v->name->name) {
            throw yy::parser::syntax_error(v->loc, "multiple definition of variable " + v->name->name + "; previously defined at " + *(i->loc.begin.filename) + ":" + std::to_string(i->loc.begin.line));
        }
    }

    for (auto i : functions) {
        if (i->name == v->name->name) {
            throw yy::parser::syntax_error(v->loc, v->name->name + " already defined as a function at " + *(i->loc.begin.filename) + ":" + std::to_string(i->loc.begin.line));
        }
    }

    variables.push_back(v);
}

void AST::Scope::merge_scope(AST::Scope* s) {
    for (auto i : s->variables) {
        push_variable(i);
    }

    for (auto i : s->functions) {
        push_function(i);
    }
}

AST::Variable* AST::Scope::get_variable(std::string name) {
    for (auto i : variables) {
        if (i->name->name == name) return i;
    }

    return NULL;
}

AST::Function* AST::Scope::get_function(std::string name) {
    for (auto i : functions) {
        if (i->name == name) return i;
    }

    return NULL;
}

void AST::Scope::push_function(AST::Function* f) {
    /* we need to check for clashes with global vars,
     * as well as name clashes with functions. */
    for (auto i : variables) {
        if (i->name->name == f->name) {
            throw yy::parser::syntax_error(f->loc, f->name + " already defined as a variable at " + *(i->loc.begin.filename) + ":" + std::to_string(i->loc.begin.line));
        }
    }

    /* then, check if it was declared already.
     * if it is already declared make sure everything matches */
    for (auto i : functions) {
        if (i->name == f->name) {
            std::vector<AST::Variable*> first_params = i->params->variables, second_params = f->params->variables;

            if (first_params.size() != second_params.size()) {
                throw yy::parser::syntax_error(f->loc, f->name + " already declared with " + std::to_string(first_params.size()) + " arguments at " + *(i->loc.begin.filename) + ":" + std::to_string(i->loc.begin.line));
            }


            for (unsigned long p = 0; p < first_params.size(); ++p) {
                if (first_params[p]->base_type != second_params[p]->base_type || first_params[p]->name->is_array != second_params[p]->name->is_array) {
                    std::string orig_type = first_params[p]->base_type + (first_params[p]->name->is_array ? "[]" : "");
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
                i->params = f->params;
                i->locals = f->locals;
                i->body = f->body;
                i->loc = f->loc;
                i->scope = f->scope;
                i->defined = true;
                return;
            }
        }
    }

    /* function not defined yet. we can just push it to the list regardless */
    functions.push_back(f);
}

void AST::Scope::write() {
    std::cout << "<Scope>\n";
    std::cout << "(variables)\n";
    for (auto i : variables) i->write();
    std::cout << "(functions)\n";
    for (auto i : functions) i->write();
    std::cout << "</Scope>\n";
}
