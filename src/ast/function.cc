#include "function.hh"
#include "scope.hh"

AST::Function::Function(location loc,
                        std::string ret_type,
                        std::string name,
                        AST::Scope* params)
    : Node(loc), name(name), ret_type(ret_type), params(params), defined(false) {}

AST::Function::Function(location loc, std::string ret_type, std::string name,
                        AST::Scope* params,
                        AST::Scope* locals,
                        std::vector<Statement*> body)
    : Node(loc), name(name), ret_type(ret_type), locals(locals), params(params), body(body), defined(true) {
    /* make sure we can merge params and locals */
    scope = new AST::Scope(loc, params, locals);
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

void AST::Function::ic_reserve_ids(Gen::CodegenState* gen) {
    ic_function_id = gen->new_function_id();

    for (auto i : body) {
        i->ic_reserve_ids(gen);
    }
}
