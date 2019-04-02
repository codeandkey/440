#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../location.hh"

typedef yy::location location;

namespace AST {
    class Node {
    public:
        Node(location& loc);
        virtual void write() = 0;
        location& loc;
    };
}
