#pragma once
#include "node.hh"
#include "variable.hh"
#include "function.hh"

/* scope is more of an abtsract structure, useful for checking for name conflicts */

namespace AST {
    class Scope : public Node {
    public:
        Scope(location loc);
        Scope(location loc, AST::Variable* first);
        Scope(location loc, std::string type, std::vector<AST::VariableName*> names);
        Scope(location loc, Scope* a, Scope* b);
        ~Scope();

        void merge_scope(Scope* s);
        void push_variable(AST::Variable* v);
        void push_function(AST::Function* f);

        AST::Variable* get_variable(std::string name);
        AST::Function* get_function(std::string name);

        void write();

        std::vector<AST::Variable*> variables;
        std::vector<AST::Function*> functions;
    };
}
