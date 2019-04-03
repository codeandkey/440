#include "function.hh"

AST::Function::Function(location loc,
                        std::string ret_type,
                        std::string name,
                        std::vector<Variable*> params)
    : Node(loc), name(name), ret_type(ret_type), params(params), defined(false) {}

AST::Function::Function(location loc, std::string ret_type, std::string name,
                        std::vector<Variable*> params,
                        std::vector<Variable*> locals,
                        std::vector<Statement*> body)
    : Node(loc), name(name), ret_type(ret_type), params(params), body(body), defined(true) {}

void AST::Function::write() {
    std::cout << "<Function name=" << name << " ret_type=" << ret_type << " defined=" << defined << ">\n";
    std::cout << "(parameters)\n";
    for (auto i : params) i->write();
    std::cout << "(locals)\n";
    for (auto i : locals) i->write();
    std::cout << "(body)\n";
    for (auto i : body) i->write();
    std::cout << "</Function>\n";
}
