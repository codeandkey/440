#include "statement.hh"

AST::Statement::Statement(location loc) : Node(loc) {}

AST::ExpressionStatement::ExpressionStatement(location loc, AST::Expression* expr) : Statement(loc), expr(expr) {}

void AST::ExpressionStatement::write() {
}
