#pragma once
#include "node.hh"
#include "variable.hh"
#include "statement.hh"
#include "../gen.hh"

namespace AST {
    class Scope;

    class Function : public Node {
    public:
        Function(location, std::string ret_type, std::string name, Scope* params);
        Function(location, std::string ret_type, std::string name, Scope* params, Scope* locals, std::vector<Statement*> body);

        void write();
        void check_types(Scope* global_scope, bool verbose);

        std::string name, ret_type;
        Scope* scope, *locals, *params;
        std::vector<Statement*> body;
        bool defined;

        /* code generation state */
        int ic_function_id;
        void ic_reserve_ids(Gen::CodegenState* gen);
    };
}
