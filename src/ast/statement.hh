#pragma once
#include "node.hh"
#include "expression.hh"

namespace AST {
    class Statement : public Node {
    public:
        Statement(location loc);
    };

    class ExpressionStatement : public Statement {
    public:
        ExpressionStatement(location loc, Expression* expr);
        void write();

        Expression* expr;
    };

    class BreakStatement : public Statement {
    public:
        BreakStatement(location);
    };

    class ContinueStatement : public Statement {
    public:
        ContinueStatement(location);
    };

    class ReturnStatement : public Statement {
    public:
        ReturnStatement(location, Expression* expr);

        void write();

        Expression* expr;
    };

    class IfStatement : public Statement {
    public:
        IfStatement(location, Expression* cond, std::vector<Statement*> body);
        IfStatement(location, Expression* cond, std::vector<Statement*> body, std::vector<Statement*> else_body);

        void write();

        bool has_else;
        Expression* cond;
        std::vector<Statement*> body, else_body;
    };

    class ForStatement : public Statement {
    public:
        ForStatement(location, Expression* init, Expression* cond, Expression* next, std::vector<Statement*> body);

        void write();

        /* 3 optional values force us to use NULL pointers when there is no expression */
        Expression* init, *cond, *next;
        std::vector<Statement*> body;
    };

    class WhileStatement : public Statement {
    public:
        WhileStatement(location, Expression* cond, std::vector<Statement*> body);

        void write();

        Expression* cond;
        std::vector<Statement*> body;
    };

    class DoWhileStatement : public Statement {
    public:
        DoWhileStatement(location, Expression* cond, std::vector<Statement*> body);

        void write();

        Expression* cond;
        std::vector<Statement*> body;
    };
}
