#pragma once
#include "node.hh"

namespace AST {
    class Statement : public Node {
    public:
        void write();
    };
}
