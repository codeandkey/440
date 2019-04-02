#pragma once
#include "node.hh"
#include "expression.hh"

namespace AST {
    class IntConst : public Expression {
    public:
        IntConst(location&, int);

        void write();
        std::string type();

        int n;
    };

    class RealConst : public Expression {
    public:
        RealConst(location&, double);

        void write();
        std::string type();

        double n;
    };

    class StrConst : public Expression {
    public:
        StrConst(location&, std::string);

        void write();
        std::string type();

        std::string val;
    };

    class CharConst : public Expression {
    public:
        CharConst(location&, char);

        void write();
        std::string type();

        char val;
    };
}
