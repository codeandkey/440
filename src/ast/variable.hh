#pragma once
#include "node.hh"
#include "expression.hh"

namespace AST {
    class VariableName : public Node {
    public:
        VariableName(location loc, std::string name);
        VariableName(location loc, std::string name, int arr_size);

        void write();

        std::string name;
        bool is_array;
        int array_size;
    };

    class Variable : public Expression {
    public:
        Variable(location loc, std::string base_type, VariableName* name);

        void write();
        std::string type();

        std::string base_type;
        VariableName* name;
    };
}
