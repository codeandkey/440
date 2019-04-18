#pragma once
#include "node.hh"
#include "../gen.hh"

namespace AST {
    class Scope;
    class Function;

    class Expression : public Node {
    public:
        Expression(location);
        virtual std::string type(Scope* global_scope, Function* func);
        virtual void ic_reserve_ids(Gen::CodegenState* gen);
    };

    class LValue : public Node {
    public:
        LValue(location, std::string name, Expression* expr = NULL);

        std::string type(Scope* global_scope, Function* func);
        void write();
        void ic_reserve_ids(Gen::CodegenState* gen);

        std::string name;
        Expression* expr;
    };

    class IntConst : public Expression {
    public:
        IntConst(location, int);

        void write();
        std::string type(Scope* global_scope, Function* func);

        int n;
    };

    class RealConst : public Expression {
    public:
        RealConst(location, double);

        void write();
        std::string type(Scope* global_scope, Function* func);

        double n;
    };

    class StrConst : public Expression {
    public:
        StrConst(location, std::string);

        void write();
        std::string type(Scope* global_scope, Function* func);

        std::string val;
    };

    class CharConst : public Expression {
    public:
        CharConst(location, char);

        void write();
        std::string type(Scope* global_scope, Function* func);

        char val;
    };

    class IdentifierExpression : public Expression {
    public:
        IdentifierExpression(location, std::string name);

        void write();
        std::string type(Scope* global_scope, Function* func);

        std::string name;
    };

    class AddressExpression : public Expression {
    public:
        AddressExpression(location, std::string name);

        void write();
        std::string type(Scope* global_scope, Function* func);

        std::string name;
    };

    class IndexExpression : public Expression {
    public:
        IndexExpression(location, std::string name, Expression* ind);

        void write();
        std::string type(Scope* global_scope, Function* func);

        std::string name;
        Expression* ind;
    };

    class CallExpression : public Expression {
    public:
        CallExpression(location, std::string name, std::vector<Expression*> args);

        void write();
        std::string type(Scope* global_scope, Function* func);

        std::string name;
        std::vector<Expression*> args;
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

        LValue* lhs;
        Type t;
        Expression* rhs;
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

        Expression* lhs, *rhs;
        Type t;
    };

    class TernaryOpExpression : public Expression {
    public:
        TernaryOpExpression(location, Expression* cond, Expression* pos, Expression* neg);

        void write();
        std::string type(Scope* global_scope, Function* func);

        Expression* cond, *pos, *neg;
    };

    class CastExpression : public Expression {
    public:
        CastExpression(location, std::string cast_type, Expression* rhs);

        void write();
        std::string type(Scope* global_scope, Function* func);

        std::string cast_type;
        Expression* rhs;
    };
}
