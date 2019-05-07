#pragma once
#include "node.hh"
#include "variable.hh"
#include "statement.hh"

namespace AST {
    class Scope;
    class Program;

    class Function : public Node {
    public:
        Function(location, std::string ret_type, std::string name, Scope* params);
        Function(location, std::string ret_type, std::string name, Scope* params, Scope* locals, std::vector<Statement*> body);
        Function(location, std::string ret_type, std::string name, Scope* params, int builtin);

        void write();
        void check_types(Scope* global_scope, bool verbose);

        std::string name, ret_type;
        Scope* scope, *locals, *params;
        std::vector<Statement*> body;
        bool defined;

        /* code generation */
        bool is_builtin;
        int function_number; /* set by AST::Program before code gen unless the function is builtin */
        void reserve(AST::Program* prg);
        std::string gen_code(Scope* global_scope);

        int local_counter = 0; /* counter for local variables, needed for array types */
        int label_counter = 0;

        std::string make_label();
    };
}
