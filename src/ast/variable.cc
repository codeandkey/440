#include "variable.hh"

AST::VariableName::VariableName(location& loc, std::string name) : Node(loc), name(name), is_array(false) {}
AST::VariableName::VariableName(location& loc, std::string name, int arr_size) : Node(loc), name(name), is_array(true), array_size(arr_size) {}

void AST::VariableName::write() {
    std::cout << "<VariableName name=\"" << name << "\" is_array=" << is_array;
    if (is_array) std::cout << " array_size=" << array_size;
    std::cout << " />\n";
}

AST::Variable::Variable(location& loc, std::string base_type, AST::VariableName* name) : Expression(loc), base_type(base_type), name(name) {}

std::string AST::Variable::type() {
    return base_type + (name->is_array ? "[]" : "");
}

void AST::Variable::write() {
    std::cout << "<Variable base_type=" << base_type << ">\n";
    name->write();
    std::cout << "</Variable>\n";
}
