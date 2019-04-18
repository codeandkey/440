#pragma once

/*
 * utils for code generation
 */

#include <string>
#include <vector>
#include <cstdint>

namespace Gen {
    class CodegenState {
    public:
        CodegenState();

        int new_global_id();
        int new_constant_id(uint32_t initializer);
        int new_function_id();

        std::string get_globals_section();
        std::string get_constants_section();
        std::string get_functions_section();
    private:
        int global_counter, constant_counter, function_counter;

        std::vector<uint32_t> constant_values;
    };
}
