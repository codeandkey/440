#include "gen.hh"

Gen::CodegenState::CodegenState() {
    global_counter = constant_counter = 0;
    function_counter = 2;
}

int Gen::CodegenState::new_global_id() {
    return global_counter++;
}

int Gen::CodegenState::new_constant_id(uint32_t initializer) {
    constant_values.push_back(initializer);
    return constant_counter++;
}

int Gen::CodegenState::new_function_id() {
    return function_counter++;
}

std::string Gen::CodegenState::get_globals_section() {
    return ".GLOBALS " + std::to_string(global_counter) + "\n";
}

std::string Gen::CodegenState::get_constants_section() {
    std::string out = ".CONSTANTS ";
    out += std::to_string(constant_counter) + "\n";
    for (auto i : constant_values) {
        char val[14];
        snprintf(val, sizeof val, "  0x%08x\n", i);
        out += val;
    }
    return out;
}

std::string Gen::CodegenState::get_functions_section() {
    return ".FUNCTIONS " + std::to_string(function_counter - 2) + "\n";
}
