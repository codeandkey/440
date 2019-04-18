#pragma once
#include "node.hh"
#include "variable.hh"
#include "function.hh"
#include "scope.hh"

#include <cstdint>

namespace AST {
    class Program : public Node {
    public:
        Program(location);

        void write();
        void push_globals(Scope* s);
        void push_function(Function*);

        void check_types(bool verbose);
        std::string generate_ir();

        Scope* scope;

        /* allocate a new constant location */
        std::string make_const_int(int v);
        std::string make_const_real(float v);
        std::string make_const_string(std::string v);

    private:
        int const_counter, function_counter;
        std::vector<uint32_t> const_values;
    };
}
