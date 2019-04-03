#pragma once
#include "node.hh"

namespace AST {
    class Expression : public Node {
    public:
        Expression(location);
        virtual std::string type() = 0;
    };
}
