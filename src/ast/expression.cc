#include "expression.hh"
#include "../parser.hh"

/* Expression base */
AST::Expression::Expression(location loc) : Node(loc) {}

std::string AST::Expression::type(Scope* global_scope, Function* func) { return "NOTYPE"; }

void AST::Expression::reserve(AST::Program* prg) {}

std::string AST::Expression::gen_code(Scope* global_scope, Function* func, bool keep_result) { return "    ; default Expression gen_code()?\n"; }

/* LValue */
AST::LValue::LValue(location loc, std::string name, Expression* expr) : Node(loc), name(name), expr(expr) {}

void AST::LValue::write() {
    std::cout << "<LValue name=" << name << ">\n";
    if (expr) expr->write();
    std::cout << "</LValue>\n";
}

std::string AST::LValue::type(Scope* global_scope, Function* func) {
    var = func->scope->get_variable(name);
    if (!var) var = global_scope->get_variable(name);

    if (!var) {
        throw yy::parser::syntax_error(loc, "unknown variable name '" + name + "'");
    }

    if (expr) {
        if (!var->name->is_array) {
            throw yy::parser::syntax_error(loc, "cannot index into non-array type '" + var->base_type + "'");
        }

        std::string ind_type = expr->type(global_scope, func);
        if (ind_type != "int") {
            throw yy::parser::syntax_error(loc, "invalid index type '" + ind_type + "'");
        }

        return var->base_type;
    }

    return var->base_type + (var->name->is_array ? "[]" : "");
}

void AST::LValue::reserve(AST::Program* prg) {
    if (expr) expr->reserve(prg);
}

std::string AST::LValue::gen_store_code(Scope* global_scope, Function* func, bool keep_result) {
    /* this code gen assumes we have the dest value on the top of the stack. */

    /* are we indexing into an array? then we need to compute the index */
    std::string out;

    if (expr) {
        if (keep_result) out += "    copy\n";
        out += expr->gen_code(global_scope, func, true);
        out += std::string("    ptrto ") + var->code_location + "\n";
        out += std::string("    move 2\n    move 2\n"); /* we have to shift around the order of the stack for pop[] */
        out += std::string("    pop") + var->base_type[0] + "[]\n";
    } else {
        if (keep_result) out += "    copy\n";
        out += std::string("    pop ") + var->code_location + "\n";
    }

    return out;
}

std::string AST::LValue::gen_retrieve_code(Scope* global_scope, Function* func, bool keep_result) {
    /* keep_result will never be false here.. */
    std::string out;

    if (expr) out += expr->gen_code(global_scope, func, keep_result);
    if (!keep_result) return out;

    if (expr) {
        /* index into the array */
        out += std::string("    ptrto ") + var->code_location + "\n";
        out += std::string("    push") + var->base_type[0] + "[]\n";
    } else {
        out += std::string("    push ") + var->code_location + "\n";
    }

    return out;
}

/* Constants */
AST::IntConst::IntConst(location loc, int n) : Expression(loc), n(n) {}
void AST::IntConst::write() { std::cout << "<IntConst n=" << n << ">\n"; }
std::string AST::IntConst::type(Scope* global_scope, Function* func) { return "int"; }

void AST::IntConst::reserve(AST::Program* prg) {
    code_location = prg->make_const_int(n);
}

std::string AST::IntConst::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    if (!keep_result) return ""; /* not actually using this */

    /* need to push the constant onto the stack */
    return "    push " + code_location + "\n";
}

AST::RealConst::RealConst(location loc, double n) : Expression(loc), n(n) {}
void AST::RealConst::write() { std::cout << "<RealConst n=" << n << ">\n"; }
std::string AST::RealConst::type(Scope* global_scope, Function* func) { return "float"; }

void AST::RealConst::reserve(AST::Program* prg) {
    code_location = prg->make_const_real(n);
}

std::string AST::RealConst::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    if (!keep_result) return ""; /* not actually using this */
    return "    push " + code_location + "\n";
}

AST::StrConst::StrConst(location loc, std::string val) : Expression(loc), val(val) {}
void AST::StrConst::write() { std::cout << "<StrConst val=\"" << val << "\">\n"; }
std::string AST::StrConst::type(Scope* global_scope, Function* func) { return "char[]"; }

std::string AST::StrConst::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    if (!keep_result) return "";
    return "    ptrto " + code_location + "\n";
}

void AST::StrConst::reserve(AST::Program* prg) {
    code_location = prg->make_const_string(val);
}

AST::CharConst::CharConst(location loc, char val) : Expression(loc), val(val) {}
void AST::CharConst::write() { std::cout << "<CharConst val='" << val << "'>\n"; }
std::string AST::CharConst::type(Scope* global_scope, Function* func) { return "char"; }

void AST::CharConst::reserve(AST::Program* prg) {
    code_location = prg->make_const_int(val);
}

std::string AST::CharConst::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    if (!keep_result) return ""; /* not actually using this */
    return "    push " + code_location + "\n";
}

/* IdentifierExpression */
AST::IdentifierExpression::IdentifierExpression(location loc, std::string name) : Expression(loc), name(name) {}

void AST::IdentifierExpression::write() {
    std::cout << "<IdentifierExpression name=\"" << name << "\" />\n";
}

std::string AST::IdentifierExpression::type(Scope* global_scope, Function* func) {
    /* search the function scope for variables, then the global scope */
    var = func->scope->get_variable(name);
    if (!var) var = global_scope->get_variable(name);

    if (!var) {
        throw yy::parser::syntax_error(loc, "unknown variable name '" + name + "'");
    }

    return var->base_type + (var->name->is_array ? "[]" : "");
}

std::string AST::IdentifierExpression::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    if (!keep_result) return "";

    if (var->name->is_array) {
        return "    ptrto " + var->code_location + "\n";
    } else {
        return "    push " + var->code_location + "\n";
    }
}

/* AddressExpression */
AST::AddressExpression::AddressExpression(location loc, std::string name) : Expression(loc), name(name) {}

void AST::AddressExpression::write() {
    std::cout << "<AddressExpression name=\"" << name << "\" />\n";
}

std::string AST::AddressExpression::type(Scope* global_scope, Function* func) {
    var = func->scope->get_variable(name);
    if (!var) var = global_scope->get_variable(name);

    if (!var) {
        throw yy::parser::syntax_error(loc, "unknown variable name '" + name + "'");
    }

    if (var->name->is_array) {
        throw yy::parser::syntax_error(loc, "cannot get address of array type '" + var->base_type + "[]'");
    }

    return var->base_type + "[]";
}

std::string AST::AddressExpression::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    if (!keep_result) return "";
    return "    ptrto " + var->code_location + "\n";
}

/* IndexExpression */
AST::IndexExpression::IndexExpression(location loc, std::string name, Expression* ind) : Expression(loc), name(name), ind(ind) {}

void AST::IndexExpression::write() {
    std::cout << "<IndexExpression name=\"" << name << "\">\n";
    ind->write();
    std::cout << "</IndexExpression>\n";
}

std::string AST::IndexExpression::type(Scope* global_scope, Function* func) {
    var = func->scope->get_variable(name);
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

void AST::IndexExpression::reserve(AST::Program* prg) {
    if (ind) ind->reserve(prg);
}

std::string AST::IndexExpression::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    /* no matter what, we need to evaluate the index. */

    std::string out = ind->gen_code(global_scope, func, keep_result);
    if (!keep_result) return out;

    /* we need the result, actually index the array */
    out += std::string("    ptrto ") + var->code_location + "\n";
    out += std::string("    push") + var->base_type[0] + "[]\n";

    return out;
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
    f = global_scope->get_function(name);

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

void AST::CallExpression::reserve(AST::Program* prg) {
    for (auto i : args) {
        i->reserve(prg);
    }
}

std::string AST::CallExpression::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    /* push arguments in order, then call the function */
    /* return value is automatically pushed for us! */

    std::string out;

    for (auto i : args) {
        out += i->gen_code(global_scope, func, true);
    }

    out += "    call " + std::to_string(f->function_number) + "\n";

    /* if the function returned, and we're not keeping it,
     * we need to pop the retval. off the stack */
    if (f->ret_type != "void" && !keep_result) {
        out += "    popx\n";
    }

    return out;
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

    operand_type = lhs_type;
    return lhs_type;
}

std::string AST::AssignmentExpression::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    /* in any assignment we always have to evaluate everything */
    /* if we're updating an existing value we want to get that first */
    /* first, get the right-hand value */

    std::string out;

    /* get the value to update if we need it */
    if (t != Type::ASSIGN) {
        out += lhs->gen_retrieve_code(global_scope, func, true);
    }

    out += rhs->gen_code(global_scope, func, true);

    switch (t) {
    case Type::ASSIGN:
        /* perform basic assignment, no need to alter the rhs */
        break;
    case Type::PLUSASSIGN:
        /* updating assignments, operate on the retrieved value */
        out += std::string("    +") + operand_type[0] + "\n";
        break;
    case Type::MINUSASSIGN:
        out += std::string("    -") + operand_type[0] + "\n";
        break;
    case Type::STARASSIGN:
        out += std::string("    *") + operand_type[0] + "\n";
        break;
    case Type::SLASHASSIGN:
        out += std::string("    /") + operand_type[0] + "\n";
        break;
    }

    out += lhs->gen_store_code(global_scope, func, keep_result);
    return out;
}

void AST::AssignmentExpression::reserve(AST::Program* prg) {
    lhs->reserve(prg);
    rhs->reserve(prg);
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

void AST::IncDecExpression::reserve(AST::Program* prg) {
    operand->reserve(prg);
}

std::string AST::IncDecExpression::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    /* increment / decrement operation */
    /* we will always update the lvalue, so, first we retrieve the contents */
    std::string out = operand->gen_retrieve_code(global_scope, func, true);

    /* now, the stack contains the value to be modified. */

    /* if we're keeping the returned value then we'll copy it before performing the store
     * if it's a pre-operation then we copy after the op
     * it it's a post-operation then we copy before the op */

    if (keep_result && !is_pre) {
        out += "    copy\n";
    }

    /* perform the operation */
    out += std::string("    ") + ((t == Type::INCR) ? "++" : "--") + operand->var->base_type[0] + "\n";

    if (keep_result && is_pre) {
        out += "    copy\n";
    }

    /* now, we store the top and then if there is a return value it will be under it. */
    out += operand->gen_store_code(global_scope, func, false);

    return out;
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

void AST::UnaryOpExpression::reserve(AST::Program* prg) {
    operand->reserve(prg);
}

std::string AST::UnaryOpExpression::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    /* we MUST evaluate the operand. */
    /* if we stop too early, then ~(foo(2)) will never call foo() */

    std::string out = operand->gen_code(global_scope, func, keep_result);

    /* 
     * however we can do some funky logic.
     * we only need the result from the operand code if we need a result from our operation.
     */

    if (!keep_result) return out;

    switch (t) {
    case Type::MINUS:
        out += std::string("    neg") + operand->type(global_scope, func)[0] + "\n";
        break;
    case Type::BANG:
        throw yy::parser::syntax_error(loc, "unary operator '!' not implemented");
        break;
    case Type::TILDE:
        out += "    flip\n";
        break;
    }

    return out;
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

    operand_type = lhs_type;

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

void AST::BinaryOpExpression::reserve(AST::Program* prg) {
    lhs->reserve(prg);
    rhs->reserve(prg);
}

std::string AST::BinaryOpExpression::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    /*
     * with binary operations we need to be very specific about how we evaluate expressions
     *
     * right now we don't need any short circuiting, but here is where we
     * may have to conditionally evaluate the second expression depending on the
     * result of the first.
     */

    std::string out = lhs->gen_code(global_scope, func, keep_result);
    out += rhs->gen_code(global_scope, func, keep_result);

    if (!keep_result) return out;

    /* 
     * it would be nice to use the goto labels for expression directly, but
     * we need to be able to evaluate binary comparisons to the correct value (char)
     * so, we just make labels and push constants which looks ugly, but
     * results in correct code
     */

    std::string tmp_label, tmp_label2; /* labels used by comparators */

    switch (t) {
    case Type::EQUALS:
        /* push a 1 if the operands are equal */
        tmp_label = func->make_label();
        tmp_label2 = func->make_label();
        out += std::string("    ==") + operand_type[0] + " " + tmp_label + "\n";
        out += "    pushv 0x0\n";
        out += "    goto " + tmp_label2 + "\n";
        out += tmp_label + ":    pushv 0x1\n";
        out += tmp_label2 + ":";
        break;
    case Type::NEQUAL:
        /* push a 1 if the operands are not equal */
        /* same as equal, we just change the operator */
        tmp_label = func->make_label();
        tmp_label2 = func->make_label();
        out += std::string("    !=") + operand_type[0] + " " + tmp_label + "\n";
        out += "    pushv 0x0\n";
        out += "    goto " + tmp_label2 + "\n";
        out += tmp_label + ":    pushv 0x1\n";
        out += tmp_label2 + ":";
        break;
    case Type::GT:
        tmp_label = func->make_label();
        tmp_label2 = func->make_label();
        out += std::string("    >") + operand_type[0] + " " + tmp_label + "\n";
        out += "    pushv 0x0\n";
        out += "    goto " + tmp_label2 + "\n";
        out += tmp_label + ":    pushv 0x1\n";
        out += tmp_label2 + ":";
        break;
    case Type::GE:
        tmp_label = func->make_label();
        tmp_label2 = func->make_label();
        out += std::string("    >=") + operand_type[0] + " " + tmp_label + "\n";
        out += "    pushv 0x0\n";
        out += "    goto " + tmp_label2 + "\n";
        out += tmp_label + ":    pushv 0x1\n";
        out += tmp_label2 + ":";
        break;
    case Type::LT:
        tmp_label = func->make_label();
        tmp_label2 = func->make_label();
        out += std::string("    <") + operand_type[0] + " " + tmp_label + "\n";
        out += "    pushv 0x0\n";
        out += "    goto " + tmp_label2 + "\n";
        out += tmp_label + ":    pushv 0x1\n";
        out += tmp_label2 + ":";
        break;
    case Type::LE:
        tmp_label = func->make_label();
        tmp_label2 = func->make_label();
        out += std::string("    <=") + operand_type[0] + " " + tmp_label + "\n";
        out += "    pushv 0x0\n";
        out += "    goto " + tmp_label2 + "\n";
        out += tmp_label + ":    pushv 0x1\n";
        out += tmp_label2 + ":";
        break;
    case Type::DPIPE:
    case Type::DAMP:
        throw yy::parser::syntax_error(loc, "conditional binary operators not implemented");
    case Type::PLUS:
        out += std::string("    +") + operand_type[0] + "\n";
        break;
    case Type::MINUS:
        out += std::string("    -") + operand_type[0] + "\n";
        break;
    case Type::STAR:
        out += std::string("    *") + operand_type[0] + "\n";
        break;
    case Type::SLASH:
        out += std::string("    /") + operand_type[0] + "\n";
        break;
    case Type::MOD:
        out += std::string("    %") + operand_type[0] + "\n";
        break;
    case Type::AMP:
        out += std::string("    &\n");
        break;
    case Type::PIPE:
        out += std::string("    |\n");
        break;
    }

    return out;
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
    cond_type = cond->type(global_scope, func);
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

void AST::TernaryOpExpression::reserve(AST::Program* prg) {
    cond->reserve(prg);
    pos->reserve(prg);
    neg->reserve(prg);
}

std::string AST::TernaryOpExpression::gen_code(Scope* scope, Function* func, bool keep_result) {
    /* short-circuited ternary op implementation */
    /* eval the condition no matter what */

    std::string output = cond->gen_code(scope, func, true);
    std::string neg_label = func->make_label(), post_neg_label = func->make_label();

    output += std::string("    ==0") + cond_type[0] + " " + neg_label + "\n";
    output += pos->gen_code(scope, func, keep_result);
    output += std::string("    goto ") + post_neg_label + "\n";
    output += neg_label + ":";
    output += neg->gen_code(scope, func, keep_result);
    output += post_neg_label + ":";

    return output;
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

std::string AST::CastExpression::gen_code(Scope* global_scope, Function* func, bool keep_result) {
    /* we have a nested expression, so we must evaluate it regardless of whether we're keeping the result */
    std::string out = rhs->gen_code(global_scope, func, keep_result);
    if (!keep_result) return out;

    std::string oper_type = rhs->type(global_scope, func);

    /* many of the casts can be no-ops */
    if (cast_type == "char") {
        if (oper_type == "float") out += "    convfi\n";
    } else if (cast_type == "int") {
        if (oper_type == "float") out += "    convfi\n";
    } else if (cast_type == "float") {
        if (oper_type == "char") out += "    convif\n";
        if (oper_type == "int") out += "    convif\n";
    }

    return out;
}

void AST::CastExpression::reserve(AST::Program* prg) {
    rhs->reserve(prg);
}
