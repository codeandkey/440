#include "statement.hh"
#include "function.hh"
#include "scope.hh"
#include "../parser.hh"

/* Statement base class */
AST::Statement::Statement(location loc) : Node(loc) {}

void AST::Statement::check_types(Scope* global_scope, Function* func, bool verbose) {
    return;
}

void AST::Statement::reserve(AST::Program* prg) {}

std::string AST::Statement::gen_code(Scope* global_scope, Function* func) {
    return "";
}

/* ExpressionStatement */
AST::ExpressionStatement::ExpressionStatement(location loc, AST::Expression* expr) : Statement(loc), expr(expr) {}

void AST::ExpressionStatement::write() {
    std::cout << "<ExpressionStatement>\n";
    expr->write();
    std::cout << "</ExpressionStatement>\n";
}

void AST::ExpressionStatement::check_types(Scope* global_scope, Function* func, bool verbose) {
    std::string expr_type = expr->type(global_scope, func);
    if (verbose) std::cout << "Expression at " << *(loc.begin.filename) << ":" << std::to_string(loc.begin.line) << " has type " << expr_type << "\n";
}

void AST::ExpressionStatement::reserve(AST::Program* prg) {
    expr->reserve(prg);
}

std::string AST::ExpressionStatement::gen_code(Scope* global_scope, Function* func) {
    return expr->gen_code(global_scope, func, false);
}

/* BreakStatement */
AST::BreakStatement::BreakStatement(location loc) : Statement(loc) {}

/* ContinueStatement */
AST::ContinueStatement::ContinueStatement(location loc) : Statement(loc) {}

/* ReturnStatement */
AST::ReturnStatement::ReturnStatement(location loc, Expression* expr) : Statement(loc), expr(expr) {}

void AST::ReturnStatement::write() {
    std::cout << "<ReturnStatement>\n";
    if (expr) expr->write();
    std::cout << "</ReturnStatement>\n";
}

void AST::ReturnStatement::check_types(Scope* scope, Function* func, bool verbose) {
    if (func->ret_type == "void" && expr) {
        throw yy::parser::syntax_error(loc, "return statement with value, in function returning void");
    }

    if (!expr && func->ret_type != "void") {
        throw yy::parser::syntax_error(loc, "return statement with no value, in function returning " + func->ret_type);
    }

    std::string expr_type = expr ? expr->type(scope, func) : "void";

    if (expr_type != func->ret_type) {
        throw yy::parser::syntax_error(loc, "mismatched types; cannot return '" + expr_type + "' from function returning " + func->ret_type);
    }
}

void AST::ReturnStatement::reserve(AST::Program* prg) {
    if (expr) expr->reserve(prg);
}

std::string AST::ReturnStatement::gen_code(Scope* scope, Function* func) {
    std::string out;
    if (expr) out = expr->gen_code(scope, func, true);
    out += "    ret\n";
    return out;
}

/* IfStatement */
AST::IfStatement::IfStatement(location loc, Expression* cond, std::vector<Statement*> body)
    : Statement(loc), has_else(false), cond(cond), body(body) {}

AST::IfStatement::IfStatement(location loc, Expression* cond, std::vector<Statement*> body, std::vector<Statement*> else_body)
    : Statement(loc), has_else(true), cond(cond), body(body), else_body(else_body) {}

void AST::IfStatement::write() {
    std::cout << "<IfStatement>\n";
    std::cout << "(condition)\n";
    cond->write();
    std::cout << "(body)\n";
    for (auto i : body) i->write();
    if (has_else) {
        std::cout << "(else_body)\n";
        for (auto i : else_body) i->write();
    }
    std::cout << "</IfStatement>\n";
}

void AST::IfStatement::check_types(Scope* global_scope, Function* func, bool verbose) {
    std::string cond_type = cond->type(global_scope, func);

    if (cond_type != "int" && cond_type != "char" && cond_type != "float") {
        throw yy::parser::syntax_error(loc, "invalid condition type " + cond_type + " in 'if' statement");
    }

    for (auto i : body) {
        i->check_types(global_scope, func, verbose);
    }

    for (auto i : else_body) {
        i->check_types(global_scope, func, verbose);
    }
}

void AST::IfStatement::reserve(AST::Program* prg) {
    cond->reserve(prg);

    for (auto i : body) {
        i->reserve(prg);
    }

    for (auto i : else_body) {
        i->reserve(prg);
    }
}

/* ForStatement */
AST::ForStatement::ForStatement(location loc, Expression* init, Expression* cond, Expression* next, std::vector<Statement*> body)
    : Statement(loc), init(init), cond(cond), next(next), body(body) {}

void AST::ForStatement::write() {
    std::cout << "<ForStatement>\n";
    if (init) {
        std::cout << "(init)\n";
        init->write();
    }
    if (cond) {
        std::cout << "(cond)\n";
        cond->write();
    }
    if (next) {
        std::cout << "(next)\n";
        next->write();
    }
    std::cout << "(body)\n";
    for (auto i : body) i->write();
    std::cout << "</ForStatement>\n";
}

void AST::ForStatement::check_types(Scope* global_scope, Function* func, bool verbose) {
    if (cond) {
        std::string cond_type = cond->type(global_scope, func);

        if (cond_type != "int" && cond_type != "char" && cond_type != "float") {
            throw yy::parser::syntax_error(loc, "invalid condition type " + cond_type + " in 'for' statement");
        }
    }

    if (init) init->type(global_scope, func);
    if (next) next->type(global_scope, func);

    for (auto i : body) {
        i->check_types(global_scope, func, verbose);
    }
}

void AST::ForStatement::reserve(AST::Program* prg) {
    if (init) init->reserve(prg);
    if (cond) cond->reserve(prg);
    if (next) next->reserve(prg);

    for (auto i : body) {
        i->reserve(prg);
    }
}

/* WhileStatement */
AST::WhileStatement::WhileStatement(location loc, Expression* cond, std::vector<Statement*> body)
    : Statement(loc), cond(cond), body(body) {}

void AST::WhileStatement::write() {
    std::cout << "<WhileStatement>\n";
    std::cout << "(cond)\n";
    cond->write();
    std::cout << "(body)\n";
    for (auto i : body) i->write();
    std::cout << "</WhileStatement>\n";
}

void AST::WhileStatement::check_types(Scope* global_scope, Function* func, bool verbose) {
    std::string cond_type = cond->type(global_scope, func);

    if (cond_type != "int" && cond_type != "char" && cond_type != "float") {
        throw yy::parser::syntax_error(loc, "invalid condition type " + cond_type + " in 'while' statement");
    }

    for (auto i : body) {
        i->check_types(global_scope, func, verbose);
    }
}

void AST::WhileStatement::reserve(AST::Program* prg) {
    cond->reserve(prg);

    for (auto i : body) {
        i->reserve(prg);
    }
}

/* DoWhileStatement */
AST::DoWhileStatement::DoWhileStatement(location loc, Expression* cond, std::vector<Statement*> body)
    : Statement(loc), cond(cond), body(body) {}

void AST::DoWhileStatement::write() {
    std::cout << "<DoWhileStatement>\n";
    std::cout << "(cond)\n";
    cond->write();
    std::cout << "(body)\n";
    for (auto i : body) i->write();
    std::cout << "</DoWhileStatement>\n";
}

void AST::DoWhileStatement::check_types(Scope* global_scope, Function* func, bool verbose) {
    std::string cond_type = cond->type(global_scope, func);

    if (cond_type != "int" && cond_type != "char" && cond_type != "float") {
        throw yy::parser::syntax_error(loc, "invalid condition type " + cond_type + " in 'while' statement");
    }

    for (auto i : body) {
        i->check_types(global_scope, func, verbose);
    }
}

void AST::DoWhileStatement::reserve(AST::Program* prg) {
    cond->reserve(prg);

    for (auto i : body) {
        i->reserve(prg);
    }
}
