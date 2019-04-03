#pragma once
#include "node.hh"
#include "variable.hh"
#include "statement.hh"

namespace AST {
    class Function : public Node {
    public:
        Function(location, std::string ret_type, std::string name, std::vector<Variable*> params);
        Function(location, std::string ret_type, std::string name, std::vector<Variable*> params, std::vector<Variable*> locals, std::vector<Statement*> body);

        void write();

        std::string name, ret_type;
        std::vector<Variable*> locals, params;
        std::vector<Statement*> body;
        bool defined;
    };
}
