#pragma once
#include "node.hh"
#include "expression.hh"

namespace AST {
    class Scope;
    class Function;
    class Program;

    class Statement : public Node {
    public:
        Statement(location loc);

        virtual void check_types(Scope* global_scope, Function* func, bool verbose);
        virtual void reserve(AST::Program* prg);
        virtual std::string gen_code(Scope* global_scope, Function* func);

        /* backpatch is just a string substitution */
        std::string backpatch(std::string code, std::string sub, std::string repl);
    };

    class ExpressionStatement : public Statement {
    public:
        ExpressionStatement(location loc, Expression* expr);
        void write();

        void check_types(Scope* global_scope, Function* func, bool verbose);
        void reserve(AST::Program* prg);
        std::string gen_code(Scope* global_scope, Function* func);

        Expression* expr;
    };

    class BreakStatement : public Statement {
    public:
        BreakStatement(location);

        std::string gen_code(Scope* scope, Function* func);
    };

    class ContinueStatement : public Statement {
    public:
        ContinueStatement(location);

        std::string gen_code(Scope* scope, Function* func);
    };

    class ReturnStatement : public Statement {
    public:
        ReturnStatement(location, Expression* expr);

        void check_types(Scope* global_scope, Function* func, bool verbose);
        void write();
        void reserve(AST::Program* prg);
        std::string gen_code(Scope* global_scope, Function* func);

        Expression* expr;
    };

    class IfStatement : public Statement {
    public:
        IfStatement(location, Expression* cond, std::vector<Statement*> body);
        IfStatement(location, Expression* cond, std::vector<Statement*> body, std::vector<Statement*> else_body);

        void write();
        void check_types(Scope* global_scope, Function* func, bool verbose);
        void reserve(AST::Program* prg);

        std::string gen_code(Scope* scope, Function* func);

        bool has_else;
        Expression* cond;
        std::vector<Statement*> body, else_body;
    };

    class ForStatement : public Statement {
    public:
        ForStatement(location, Expression* init, Expression* cond, Expression* next, std::vector<Statement*> body);

        void write();
        void check_types(Scope* global_scope, Function* func, bool verbose);
        void reserve(AST::Program* prg);

        std::string gen_code(Scope* scope, Function* func);

        /* 3 optional values force us to use NULL pointers when there is no expression */
        Expression* init, *cond, *next;
        std::vector<Statement*> body;
    };

    class WhileStatement : public Statement {
    public:
        WhileStatement(location, Expression* cond, std::vector<Statement*> body);

        void write();
        void check_types(Scope* global_scope, Function* func, bool verbose);
        void reserve(AST::Program* prg);

        std::string gen_code(Scope* scope, Function* func);

        Expression* cond;
        std::vector<Statement*> body;
    };

    class DoWhileStatement : public Statement {
    public:
        DoWhileStatement(location, Expression* cond, std::vector<Statement*> body);

        void write();
        void check_types(Scope* global_scope, Function* func, bool verbose);
        void reserve(AST::Program* prg);

        std::string gen_code(Scope* scope, Function* func);

        Expression* cond;
        std::vector<Statement*> body;
    };
}
