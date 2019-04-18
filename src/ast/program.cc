#include "program.hh"
#include "../parser.hh"

AST::Program::Program(location loc) : Node(loc), const_counter(0) {
    scope = new AST::Scope(loc);

    /* here we should initialize the builtin functions */
    push_function(new Function(loc, "int", "getchar", new Scope(loc), 0));
    push_function(new Function(loc, "int", "putchar", new Scope(loc, new Variable(loc, "int", new VariableName(loc, "c"))), 1));
}

void AST::Program::write() {
    std::cout << "<Program>\n";
    scope->write();
    std::cout << "</Program>\n";
}

void AST::Program::push_globals(AST::Scope* s) {
    scope->merge_scope(s);
}

void AST::Program::push_function(Function* f) {
    scope->push_function(f);
}

void AST::Program::check_types(bool verbose) {
    for (auto i : scope->functions) {
        i->check_types(scope, verbose);
    }
}

std::string AST::Program::generate_ir() {
    std::string output = "; compiler build ";
    output += __DATE__;
    output += " ";
    output += __TIME__;
    output += "\n";

    /* 0. reserve global locations */
    int global_counter = 0;
    for (auto i : scope->variables) {
        i->code_location = "G" + std::to_string(global_counter++);
    }

    /* see how many builtins we have */
    int num_builtins = 0;
    for (auto i : scope->functions) {
        if (i->is_builtin) ++num_builtins;
    }

    /* 1. reserve all other locations */
    function_counter = 0;
    for (auto i : scope->functions) {
        if (i->is_builtin) continue; /* skip builtins */
        i->function_number = function_counter++ + num_builtins;
        i->reserve(this);
    }

    /* output constant count */
    output += ".CONSTANTS " + std::to_string(const_counter) + "\n";

    /* output constant values if any */
    for (auto i : const_values) {
        char buf[11] = {0};
        snprintf(buf, sizeof buf, "0x%08x", i);
        output += "  " + std::string(buf) + "\n";
    }

    /* output global count */
    output += "\n.GLOBALS " + std::to_string(global_counter) + "\n";

    /* output function count */
    output += "\n.FUNCTIONS " + std::to_string(function_counter) + "\n";

    /* for each function, print generated code */
    for (auto i : scope->functions) {
        if (i->is_builtin) continue;
        output += "\n" + i->gen_code(scope);
    }

    return output;
}

std::string AST::Program::make_const_int(int v) {
    const_values.push_back(v);
    return "C" + std::to_string(const_counter++);
}

std::string AST::Program::make_const_real(float v) {
    const_values.push_back(*((uint32_t*) &v));
    return "C" + std::to_string(const_counter++);
}

std::string AST::Program::make_const_string(std::string v) {
    /* we make multiple constants and return the ref to the first one */
    /* break the string into chunks of 4 bytes */
    std::string ret = "C" + std::to_string(const_counter);
    while (v.size()) {
        int num = v.size();
        if (num > 4) num = 4;

        char bytes[4] = {0};
        for (int i = 0; i < num; ++i) {
            bytes[i] = v[i];
        }

        v.erase(0, 4);
        uint32_t val = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);

        const_counter++;
        const_values.push_back(val);
    }

    return ret;
}
