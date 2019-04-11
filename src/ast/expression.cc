#include "expression.hh"

/* Expression base */
AST::Expression::Expression(location loc) : Node(loc) {}

std::string AST::Expression::type() { return "NOTYPE"; }

/* LValue */
AST::LValue::LValue(location loc, std::string name, Expression* expr) : Node(loc), name(name), expr(expr) {}

void AST::LValue::write() {
    std::cout << "<LValue name=" << name << ">\n";
    if (expr) expr->write();
    std::cout << "</LValue>\n";
}

std::string AST::LValue::type() {
    return "NOTYPE";
}

/* Constants */
AST::IntConst::IntConst(location loc, int n) : Expression(loc), n(n) {}
void AST::IntConst::write() { std::cout << "<IntConst n=" << n << ">\n"; }
std::string AST::IntConst::type() { return "int"; }

AST::RealConst::RealConst(location loc, double n) : Expression(loc), n(n) {}
void AST::RealConst::write() { std::cout << "<RealConst n=" << n << ">\n"; }
std::string AST::RealConst::type() { return "float"; }

AST::StrConst::StrConst(location loc, std::string val) : Expression(loc), val(val) {}
void AST::StrConst::write() { std::cout << "<StrConst val=\"" << val << "\">\n"; }
std::string AST::StrConst::type() { return "char[]"; }

AST::CharConst::CharConst(location loc, char val) : Expression(loc), val(val) {}
void AST::CharConst::write() { std::cout << "<CharConst val='" << val << "'>\n"; }
std::string AST::CharConst::type() { return "char"; }

/* IdentifierExpression */
AST::IdentifierExpression::IdentifierExpression(location loc, std::string name) : Expression(loc), name(name) {}

void AST::IdentifierExpression::write() {
    std::cout << "<IdentifierExpression name=\"" << name << "\" />\n";
}

std::string AST::IdentifierExpression::type() {
    return "NOTYPE";
}

/* AddressExpression */
AST::AddressExpression::AddressExpression(location loc, std::string name) : Expression(loc), name(name) {}

void AST::AddressExpression::write() {
    std::cout << "<AddressExpression name=\"" << name << "\" />\n";
}

std::string AST::AddressExpression::type() {
    return "NOTYPE";
}

/* IndexExpression */
AST::IndexExpression::IndexExpression(location loc, std::string name, Expression* ind) : Expression(loc), name(name), ind(ind) {}

void AST::IndexExpression::write() {
    std::cout << "<IndexExpression name=\"" << name << "\">\n";
    ind->write();
    std::cout << "</IndexExpression>\n";
}

std::string AST::IndexExpression::type() {
    return "NOTYPE";
}

/* CallExpression */
AST::CallExpression::CallExpression(location loc, std::string name, std::vector<Expression*> args) : Expression(loc), name(name), args(args) {}

void AST::CallExpression::write() {
    std::cout << "<CallExpression name=\"" << name << "\">\n";
    for (auto i : args) i->write();
    std::cout << "</CallExpression>\n";
}

std::string AST::CallExpression::type() {
    return "NOTYPE";
}

/* AssignmentExpression */
AST::AssignmentExpression::AssignmentExpression(location loc, LValue* lhs, Type t, Expression* rhs)
    : Expression(loc), lhs(lhs), t(t), rhs(rhs) {}

void AST::AssignmentExpression::write() {
    std::cout << "<AssignmentExpression op=" << (int) t << ">\n";
    std::cout << "(lhs)\n";
    lhs->write();
    std::cout << "(rhs)\n";
    rhs->write();
    std::cout << "</AssignmentExpression>\n";
}

std::string AST::AssignmentExpression::type() {
    return "NOTYPE";
}

/* IncDecExpresion */
AST::IncDecExpression::IncDecExpression(location loc, LValue* operand, Type t, bool is_pre)
    : Expression(loc), operand(operand), t(t), is_pre(is_pre) {}

void AST::IncDecExpression::write() {
    std::cout << "<IncDecExpression type=" << (int) t << " is_pre=" << is_pre << ">\n";
    operand->write();
    std::cout << "</IncDecExpression>\n";
}

std::string AST::IncDecExpression::type() {
    return "NOTYPE";
}

/* UnaryOpExpresion */
AST::UnaryOpExpression::UnaryOpExpression(location loc, Expression* operand, Type t)
    : Expression(loc), operand(operand), t(t) {}

void AST::UnaryOpExpression::write() {
    std::cout << "<UnaryOpExpression type=" << (int) t << ">\n";
    operand->write();
    std::cout << "</UnaryOpExpression>\n";
}

std::string AST::UnaryOpExpression::type() {
    return "NOTYPE";
}

/* BinaryOpExpresion */
AST::BinaryOpExpression::BinaryOpExpression(location loc, Expression* lhs, Expression* rhs, Type t)
    : Expression(loc), lhs(lhs), rhs(rhs), t(t) {}

void AST::BinaryOpExpression::write() {
    std::cout << "<BinaryOpExpression type=" << (int) t << ">\n";
    std::cout << "(lhs)\n";
    lhs->write();
    std::cout << "(rhs)\n";
    rhs->write();
    std::cout << "</BinaryOpExpression>\n";
}

std::string AST::BinaryOpExpression::type() {
    return "NOTYPE";
}

/* TernaryOpExpresion */
AST::TernaryOpExpression::TernaryOpExpression(location loc, Expression* cond, Expression* pos, Expression* neg)
    : Expression(loc), cond(cond), pos(pos), neg(neg) {}

void AST::TernaryOpExpression::write() {
    std::cout << "<TernaryOpExpression>\n";
    std::cout << "(cond)\n";
    cond->write();
    std::cout << "(pos)\n";
    pos->write();
    std::cout << "(neg)\n";
    neg->write();
    std::cout << "</TernaryOpExpression>\n";
}

std::string AST::TernaryOpExpression::type() {
    return "NOTYPE";
}

/* CastExpresion */
AST::CastExpression::CastExpression(location loc, std::string cast_type, Expression* rhs)
    : Expression(loc), cast_type(cast_type), rhs(rhs) {}

void AST::CastExpression::write() {
    std::cout << "<CastExpression type=" << cast_type << ">\n";
    rhs->write();
    std::cout << "</CastExpression>\n";
}

std::string AST::CastExpression::type() {
    return "NOTYPE";
}
