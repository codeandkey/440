#pragma once
#include "node.hh"
#include "variable.hh"
#include "function.hh"

namespace AST {
    class Program : public Node {
    public:
        Program(location);

        void write();
        void push_globals(std::vector<Variable*>);
        void push_function(Function*);

        std::vector<Function*> functions;
        std::vector<Variable*> globals;
    };
}
