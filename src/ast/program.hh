#pragma once
#include "node.hh"
#include "variable.hh"
#include "function.hh"
#include "scope.hh"

namespace AST {
    class Program : public Node {
    public:
        Program(location);

        void write();
        void push_globals(Scope* s);
        void push_function(Function*);

        void check_types(bool verbose);

        Scope* scope;
    };
}
