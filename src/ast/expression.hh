#pragma once
#include "node.hh"

namespace AST {
    class Scope;
    class Function;
    class Program;
    class Variable;

    class Expression : public Node {
    public:
        Expression(location);
        virtual std::string type(Scope* global_scope, Function* func);
        virtual void reserve(AST::Program* prg);

        /*
         * gen_code()
         *
         * generate code for an expression
         * the value of the expression is always pushed onto the stack.
         *
         * this eliminates any need for temporary variables
         * keep_result determines if an evaluation result should be left on the stack
         */

        virtual std::string gen_code(Scope* global_scope, Function* func, bool keep_result);
    };

    class LValue : public Node {
    public:
        LValue(location, std::string name, Expression* expr = NULL);

        std::string type(Scope* global_scope, Function* func);
        void write();
        void reserve(AST::Program* prg);

        /* LValue code gen works a little differently -- we only generate code elsewhere when we need to store something in one */
        std::string gen_store_code(Scope* global_scope, Function* func, bool keep_result);
        std::string gen_retrieve_code(Scope* global_scope, Function* func, bool keep_result);

        std::string name;
        Expression* expr;
        Variable* var;
    };

    class IntConst : public Expression {
    public:
        IntConst(location, int);

        void write();
        std::string type(Scope* global_scope, Function* func);

        void reserve(AST::Program* prg);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);

        int n;
        std::string code_location;
    };

    class RealConst : public Expression {
    public:
        RealConst(location, double);

        void write();
        std::string type(Scope* global_scope, Function* func);
        void reserve(AST::Program* prg);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);

        double n;
        std::string code_location;
    };

    class StrConst : public Expression {
    public:
        StrConst(location, std::string);

        void write();
        std::string type(Scope* global_scope, Function* func);
        void reserve(AST::Program* prg);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);

        std::string val;
        std::string code_location;
    };

    class CharConst : public Expression {
    public:
        CharConst(location, char);

        void write();
        std::string type(Scope* global_scope, Function* func);
        void reserve(AST::Program* prg);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);

        char val;
        std::string code_location;
    };

    class IdentifierExpression : public Expression {
    public:
        IdentifierExpression(location, std::string name);

        void write();
        std::string type(Scope* global_scope, Function* func);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);

        std::string name;
        Variable* var;
    };

    class AddressExpression : public Expression {
    public:
        AddressExpression(location, std::string name);

        void write();
        std::string type(Scope* global_scope, Function* func);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);

        std::string name;
        Variable* var;
    };

    class IndexExpression : public Expression {
    public:
        IndexExpression(location, std::string name, Expression* ind);

        void write();
        std::string type(Scope* global_scope, Function* func);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);
        void reserve(AST::Program* prg);

        std::string name;
        Expression* ind;
        Variable* var;
    };

    class CallExpression : public Expression {
    public:
        CallExpression(location, std::string name, std::vector<Expression*> args);

        void write();
        std::string type(Scope* global_scope, Function* func);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);
        void reserve(AST::Program* prg);

        std::string name;
        std::vector<Expression*> args;

        Function* f;
    };

    class AssignmentExpression : public Expression {
    public:
        enum class Type {
            ASSIGN,
            PLUSASSIGN,
            MINUSASSIGN,
            STARASSIGN,
            SLASHASSIGN,
        };

        AssignmentExpression(location, LValue* lhs, Type t, Expression* rhs);

        void write();
        std::string type(Scope* global_scope, Function* func);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);
        void reserve(AST::Program* prg);

        LValue* lhs;
        Type t;
        Expression* rhs;
        std::string operand_type;
    };

    class IncDecExpression : public Expression {
    public:
        enum class Type {
            INCR,
            DECR,
        };

        IncDecExpression(location, LValue* operand, Type t, bool op_on_left);

        void write();
        std::string type(Scope* global_scope, Function* func);
        void reserve(AST::Program* prg);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);

        LValue* operand;
        Type t;
        bool is_pre;
    };

    class UnaryOpExpression : public Expression {
    public:
        enum class Type : int {
            MINUS,
            BANG,
            TILDE,
        };

        UnaryOpExpression(location, Expression* operand, Type t);

        void write();
        std::string type(Scope* global_scope, Function* func);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);
        void reserve(AST::Program* prg);

        Expression* operand;
        Type t;
    };

    class BinaryOpExpression : public Expression {
    public:
        enum class Type : int {
            EQUALS,
            NEQUAL,
            GT,
            GE,
            LT,
            LE,
            PLUS,
            MINUS,
            STAR,
            SLASH,
            MOD,
            PIPE,
            AMP,
            DPIPE,
            DAMP,
        };

        BinaryOpExpression(location, Expression* lhs, Expression* rhs, Type t);

        void write();
        std::string type(Scope* global_scope, Function* func);
        void reserve(AST::Program* prg);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);

        Expression* lhs, *rhs;
        Type t;
        std::string operand_type;
    };

    class TernaryOpExpression : public Expression {
    public:
        TernaryOpExpression(location, Expression* cond, Expression* pos, Expression* neg);

        void write();
        std::string type(Scope* global_scope, Function* func);
        void reserve(AST::Program* prg);

        Expression* cond, *pos, *neg;
    };

    class CastExpression : public Expression {
    public:
        CastExpression(location, std::string cast_type, Expression* rhs);

        void write();
        std::string type(Scope* global_scope, Function* func);
        void reserve(AST::Program* prg);
        std::string gen_code(Scope* global_scope, Function* func, bool keep_result);

        std::string cast_type;
        Expression* rhs;
    };
}
