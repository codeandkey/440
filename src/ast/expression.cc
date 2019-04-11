#include "expression.hh"
#include "../parser.hh"

/* Expression base */
AST::Expression::Expression(location loc) : Node(loc) {}

std::string AST::Expression::type(Scope* global_scope, Function* func) { return "NOTYPE"; }

/* LValue */
AST::LValue::LValue(location loc, std::string name, Expression* expr) : Node(loc), name(name), expr(expr) {}

void AST::LValue::write() {
    std::cout << "<LValue name=" << name << ">\n";
    if (expr) expr->write();
    std::cout << "</LValue>\n";
}

std::string AST::LValue::type(Scope* global_scope, Function* func) {
    AST::Variable* var = func->scope->get_variable(name);
    if (!var) var = global_scope->get_variable(name);

    if (!var) {
        throw yy::parser::syntax_error(loc, "unknown variable name '" + name + "'");
    }

    if (expr) {
        if (!var->name->is_array) {
            throw yy::parser::syntax_error(loc, "cannot index into non-array type '" + var->base_type + "'");
        }

        return var->base_type;
    }

    return var->base_type + (var->name->is_array ? "[]" : "");
}

/* Constants */
AST::IntConst::IntConst(location loc, int n) : Expression(loc), n(n) {}
void AST::IntConst::write() { std::cout << "<IntConst n=" << n << ">\n"; }
std::string AST::IntConst::type(Scope* global_scope, Function* func) { return "int"; }

AST::RealConst::RealConst(location loc, double n) : Expression(loc), n(n) {}
void AST::RealConst::write() { std::cout << "<RealConst n=" << n << ">\n"; }
std::string AST::RealConst::type(Scope* global_scope, Function* func) { return "float"; }

AST::StrConst::StrConst(location loc, std::string val) : Expression(loc), val(val) {}
void AST::StrConst::write() { std::cout << "<StrConst val=\"" << val << "\">\n"; }
std::string AST::StrConst::type(Scope* global_scope, Function* func) { return "char[]"; }

AST::CharConst::CharConst(location loc, char val) : Expression(loc), val(val) {}
void AST::CharConst::write() { std::cout << "<CharConst val='" << val << "'>\n"; }
std::string AST::CharConst::type(Scope* global_scope, Function* func) { return "char"; }

/* IdentifierExpression */
AST::IdentifierExpression::IdentifierExpression(location loc, std::string name) : Expression(loc), name(name) {}

void AST::IdentifierExpression::write() {
    std::cout << "<IdentifierExpression name=\"" << name << "\" />\n";
}

std::string AST::IdentifierExpression::type(Scope* global_scope, Function* func) {
    /* search the function scope for variables, then the global scope */
    AST::Variable* var = func->scope->get_variable(name);
    if (!var) var = global_scope->get_variable(name);

    if (!var) {
        throw yy::parser::syntax_error(loc, "unknown variable name '" + name + "'");
    }

    return var->base_type + (var->name->is_array ? "[]" : "");
}

/* AddressExpression */
AST::AddressExpression::AddressExpression(location loc, std::string name) : Expression(loc), name(name) {}

void AST::AddressExpression::write() {
    std::cout << "<AddressExpression name=\"" << name << "\" />\n";
}

std::string AST::AddressExpression::type(Scope* global_scope, Function* func) {
    AST::Variable* var = func->scope->get_variable(name);
    if (!var) var = global_scope->get_variable(name);

    if (!var) {
        throw yy::parser::syntax_error(loc, "unknown variable name '" + name + "'");
    }

    if (var->name->is_array) {
        throw yy::parser::syntax_error(loc, "cannot get address of array type '" + var->base_type + "[]'");
    }

    return var->base_type + "[]";
}

/* IndexExpression */
AST::IndexExpression::IndexExpression(location loc, std::string name, Expression* ind) : Expression(loc), name(name), ind(ind) {}

void AST::IndexExpression::write() {
    std::cout << "<IndexExpression name=\"" << name << "\">\n";
    ind->write();
    std::cout << "</IndexExpression>\n";
}

std::string AST::IndexExpression::type(Scope* global_scope, Function* func) {
    AST::Variable* var = func->scope->get_variable(name);
    if (!var) var = global_scope->get_variable(name);

    if (!var) {
        throw yy::parser::syntax_error(loc, "unknown variable name '" + name + "'");
    }

    if (!var->name->is_array) {
        throw yy::parser::syntax_error(loc, "cannot index into non-array type '" + var->base_type + "'");
    }

    std::string ind_type = ind->type(global_scope, func);

    if (ind_type != "int" && ind_type != "float") {
        throw yy::parser::syntax_error(loc, "cannot index into array with non-integer type '" + ind_type + "'");
    }

    return var->base_type;
}

/* CallExpression */
AST::CallExpression::CallExpression(location loc, std::string name, std::vector<Expression*> args) : Expression(loc), name(name), args(args) {}

void AST::CallExpression::write() {
    std::cout << "<CallExpression name=\"" << name << "\">\n";
    for (auto i : args) i->write();
    std::cout << "</CallExpression>\n";
}

std::string AST::CallExpression::type(Scope* global_scope, Function* func) {
    /* find function reference */
    AST::Function* f = global_scope->get_function(name);

    if (!f) {
        throw yy::parser::syntax_error(loc, "unknown function '" + name + "'");
    }

    /* check parameters match up */
    std::vector<AST::Variable*> params = f->params->variables;

    if (args.size() != params.size()) {
        throw yy::parser::syntax_error(loc, "incorrect number of arguments to '" + name + "'; expected " + std::to_string(params.size()) + ", got " + std::to_string(args.size()));
    }

    for (int i = 0; i < (int) args.size(); ++i) {
        std::string atype = args[i]->type(global_scope, func);
        std::string ptype = params[i]->base_type + (params[i]->name->is_array ? "[]" : "");

        if (atype != ptype) {
            ++i;
            throw yy::parser::syntax_error(loc, "type mismatch in argument " + std::to_string(i) + " of '" + name + "'; expected " + ptype + ", got " + atype);
        }
    }

    return f->ret_type;
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

std::string AST::AssignmentExpression::type(Scope* global_scope, Function* func) {
    std::string lhs_type = lhs->type(global_scope, func);
    std::string rhs_type = rhs->type(global_scope, func);

    if (lhs_type != rhs_type) {
        throw yy::parser::syntax_error(loc, "cannot assign " + rhs_type + " to " + lhs_type + " lvalue");
    }

    if (lhs_type != "char" && lhs_type != "int" && lhs_type != "float") {
        throw yy::parser::syntax_error(loc, "invalid assignment type " + lhs_type);
    }

    return lhs_type;
}

/* IncDecExpresion */
AST::IncDecExpression::IncDecExpression(location loc, LValue* operand, Type t, bool is_pre)
    : Expression(loc), operand(operand), t(t), is_pre(is_pre) {}

void AST::IncDecExpression::write() {
    std::cout << "<IncDecExpression type=" << (int) t << " is_pre=" << is_pre << ">\n";
    operand->write();
    std::cout << "</IncDecExpression>\n";
}

std::string AST::IncDecExpression::type(Scope* global_scope, Function* func) {
    std::string operand_type = operand->type(global_scope, func);

    if (operand_type != "char" && operand_type != "int" && operand_type != "float") {
        throw yy::parser::syntax_error(loc, "invalid type to increment/decrement: " + operand_type);
    }

    return operand_type;
}

/* UnaryOpExpresion */
AST::UnaryOpExpression::UnaryOpExpression(location loc, Expression* operand, Type t)
    : Expression(loc), operand(operand), t(t) {}

void AST::UnaryOpExpression::write() {
    std::cout << "<UnaryOpExpression type=" << (int) t << ">\n";
    operand->write();
    std::cout << "</UnaryOpExpression>\n";
}

std::string AST::UnaryOpExpression::type(Scope* global_scope, Function* func) {
    std::string operand_type = operand->type(global_scope, func);

    if (operand_type != "char" && operand_type != "int" && operand_type != "float") {
        throw yy::parser::syntax_error(loc, "invalid type '" + operand_type + "' to unary operator");
    }

    switch (t) {
    case Type::MINUS:
        return operand_type;
    case Type::BANG:
        return "char";
    case Type::TILDE:
        if (operand_type == "float") {
            throw yy::parser::syntax_error(loc, "unary '~' cannot be used on float types");
        }
        return operand_type;
    }

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

std::string AST::BinaryOpExpression::type(Scope* global_scope, Function* func) {
    std::string lhs_type = lhs->type(global_scope, func);
    std::string rhs_type = rhs->type(global_scope, func);

    if (lhs_type != rhs_type) {
        throw yy::parser::syntax_error(loc, "left-hand binary operand type " + lhs_type + " does not match right-hand type " + rhs_type);
    }

    switch (t) {
    case Type::EQUALS:
    case Type::NEQUAL:
    case Type::GT:
    case Type::GE:
    case Type::LT:
    case Type::LE:
    case Type::DPIPE:
    case Type::DAMP:
        return "char";
    default:
        return lhs_type;
    }
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

std::string AST::TernaryOpExpression::type(Scope* global_scope, Function* func) {
    std::string cond_type = cond->type(global_scope, func);
    std::string pos_type = pos->type(global_scope, func);
    std::string neg_type = neg->type(global_scope, func);

    if (cond_type != "char" && cond_type != "int" && cond_type != "float") {
        throw yy::parser::syntax_error(loc, "invalid type " + cond_type + " for ternary operator condition");
    }

    if (pos_type != neg_type) {
        throw yy::parser::syntax_error(loc, "ternary operand type " + pos_type + " does not match alternate operand type " + neg_type);
    }

    return pos_type;
}

/* CastExpresion */
AST::CastExpression::CastExpression(location loc, std::string cast_type, Expression* rhs)
    : Expression(loc), cast_type(cast_type), rhs(rhs) {}

void AST::CastExpression::write() {
    std::cout << "<CastExpression type=" << cast_type << ">\n";
    rhs->write();
    std::cout << "</CastExpression>\n";
}

std::string AST::CastExpression::type(Scope* global_scope, Function* func) {
    std::string oper_type = rhs->type(global_scope, func);

    if (cast_type == "char") {
        if (oper_type == "char") return cast_type;
        if (oper_type == "int") return cast_type;
        if (oper_type == "float") return cast_type;
    } else if (cast_type == "int") {
        if (oper_type == "char") return cast_type;
        if (oper_type == "int") return cast_type;
        if (oper_type == "float") return cast_type;
    } else if (cast_type == "float") {
        if (oper_type == "char") return cast_type;
        if (oper_type == "int") return cast_type;
        if (oper_type == "float") return cast_type;
    }

    throw yy::parser::syntax_error(loc, "cannot cast " + oper_type + " to " + cast_type);
    return "NOTYPE";
}
