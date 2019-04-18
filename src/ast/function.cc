#include "function.hh"
#include "scope.hh"
#include "../parser.hh"

AST::Function::Function(location loc,
                        std::string ret_type,
                        std::string name,
                        AST::Scope* params)
    : Node(loc), name(name), ret_type(ret_type), params(params), defined(false) {
    /* not a bulitin */
    is_builtin = false;
}

AST::Function::Function(location loc,
                        std::string ret_type,
                        std::string name,
                        AST::Scope* params,
                        int builtin)
    : Node(loc), name(name), ret_type(ret_type), params(params), defined(false) {
    /* builtin declaration! */
    function_number = builtin;
    is_builtin = true;
}

AST::Function::Function(location loc, std::string ret_type, std::string name,
                        AST::Scope* params,
                        AST::Scope* locals,
                        std::vector<Statement*> body)
    : Node(loc), name(name), ret_type(ret_type), locals(locals), params(params), body(body), defined(true) {
    /* make sure we can merge params and locals */
    scope = new AST::Scope(loc, params, locals);

    /* not a builtin */
    is_builtin = false;
}



void AST::Function::write() {
    std::cout << "<Function name=" << name << " ret_type=" << ret_type << " defined=" << defined << ">\n";
    std::cout << "(parameters)\n";
    params->write();
    std::cout << "(locals)\n";
    locals->write();
    std::cout << "(scope)\n";
    scope->write();
    std::cout << "(body)\n";
    for (auto i : body) i->write();
    std::cout << "</Function>\n";
}

void AST::Function::check_types(AST::Scope* global_scope, bool verbose) {
    if (!defined) return;

    for (auto i : body) {
        i->check_types(global_scope, this, verbose);
    }
}

void AST::Function::reserve(AST::Program* prg) {
    /* function reservation */

    /* 0. reserve parameter locations */
    local_counter = 0;
    for (auto i : params->variables) {
        i->code_location = "L" + std::to_string(local_counter++);
    }

    /* 1. reserve local locations */
    for (auto i : locals->variables) {
        int num_slots = 1;

        if (i->name->is_array) {
            num_slots = i->name->array_size;

            if (i->base_type == "char") {
                num_slots = (num_slots + 1) / 4;
            }
        }

        i->code_location = "L" + std::to_string(local_counter);
        local_counter += num_slots;
    }

    /* 2. walk statements for constants */
    for (auto i : body) {
        i->reserve(prg);
    }
}

std::string AST::Function::gen_code(Scope* global_scope) {
    /* output function info */
    std::string output = ".FUNC " + std::to_string(function_number) + " " + name + "\n";

    output += "  .params " + std::to_string(params->variables.size()) + "\n";
    output += std::string("  .return ") + ((ret_type == "void") ? "0 \n" : "1 \n");
    output += "  .locals " + std::to_string(local_counter) + "\n";

    /* output statement code */
    for (auto i : body) {
        output += i->gen_code(global_scope, this);
    }

    /* if we are supposed to return something, make sure we do.
     * a function with a proper return statement will never use this instruction */
    if (ret_type != "void") {
        output += "    pushv 0x0\n";
    }

    output += "    ret\n.end FUNC\n";
    return output;
}
