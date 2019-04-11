#include "statement.hh"

/* Statement base class */
AST::Statement::Statement(location loc) : Node(loc) {}

/* ExpressionStatement */
AST::ExpressionStatement::ExpressionStatement(location loc, AST::Expression* expr) : Statement(loc), expr(expr) {}

void AST::ExpressionStatement::write() {
    std::cout << "<ExpressionStatement>\n";
    expr->write();
    std::cout << "</ExpressionStatement>\n";
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
