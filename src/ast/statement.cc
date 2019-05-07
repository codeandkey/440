#include "statement.hh"
#include "function.hh"
#include "scope.hh"
#include "../parser.hh"

/* Statement base class */
AST::Statement::Statement(location loc) : Node(loc) {}

void AST::Statement::check_types(Scope* global_scope, Function* func, bool verbose) {
    return;
}

void AST::Statement::reserve(AST::Program* prg) {}

std::string AST::Statement::gen_code(Scope* global_scope, Function* func) {
    return "";
}

std::string AST::Statement::backpatch(std::string code, std::string sub, std::string repl) {
    size_t ind = 0;

    for (;;) {
        ind = code.find(sub, ind);
        if (ind == std::string::npos) break;
        code.replace(ind, sub.size(), repl);
        ind += sub.size();
    }

    return code;
}

/* ExpressionStatement */
AST::ExpressionStatement::ExpressionStatement(location loc, AST::Expression* expr) : Statement(loc), expr(expr) {}

void AST::ExpressionStatement::write() {
    std::cout << "<ExpressionStatement>\n";
    expr->write();
    std::cout << "</ExpressionStatement>\n";
}

void AST::ExpressionStatement::check_types(Scope* global_scope, Function* func, bool verbose) {
    std::string expr_type = expr->type(global_scope, func);
    if (verbose) std::cout << "Expression at " << *(loc.begin.filename) << ":" << std::to_string(loc.begin.line) << " has type " << expr_type << "\n";
}

void AST::ExpressionStatement::reserve(AST::Program* prg) {
    expr->reserve(prg);
}

std::string AST::ExpressionStatement::gen_code(Scope* global_scope, Function* func) {
    return expr->gen_code(global_scope, func, false);
}

/* BreakStatement */
AST::BreakStatement::BreakStatement(location loc) : Statement(loc) {}

std::string AST::BreakStatement::gen_code(Scope* scope, Function* func) {
    return "    goto <POSTLOOP>\n";
}

/* ContinueStatement */
AST::ContinueStatement::ContinueStatement(location loc) : Statement(loc) {}

std::string AST::ContinueStatement::gen_code(Scope* scope, Function* func) {
    return "    goto <PRELOOP>\n";
}

/* ReturnStatement */
AST::ReturnStatement::ReturnStatement(location loc, Expression* expr) : Statement(loc), expr(expr) {}

void AST::ReturnStatement::write() {
    std::cout << "<ReturnStatement>\n";
    if (expr) expr->write();
    std::cout << "</ReturnStatement>\n";
}

void AST::ReturnStatement::check_types(Scope* scope, Function* func, bool verbose) {
    if (func->ret_type == "void" && expr) {
        throw yy::parser::syntax_error(loc, "return statement with value, in function returning void");
    }

    if (!expr && func->ret_type != "void") {
        throw yy::parser::syntax_error(loc, "return statement with no value, in function returning " + func->ret_type);
    }

    std::string expr_type = expr ? expr->type(scope, func) : "void";

    if (expr_type != func->ret_type) {
        throw yy::parser::syntax_error(loc, "mismatched types; cannot return '" + expr_type + "' from function returning " + func->ret_type);
    }
}

void AST::ReturnStatement::reserve(AST::Program* prg) {
    if (expr) expr->reserve(prg);
}

std::string AST::ReturnStatement::gen_code(Scope* scope, Function* func) {
    std::string out;
    if (expr) out = expr->gen_code(scope, func, true);
    out += "    ret\n";
    return out;
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

void AST::IfStatement::check_types(Scope* global_scope, Function* func, bool verbose) {
    std::string cond_type = cond->type(global_scope, func);

    if (cond_type != "int" && cond_type != "char" && cond_type != "float") {
        throw yy::parser::syntax_error(loc, "invalid condition type " + cond_type + " in 'if' statement");
    }

    for (auto i : body) {
        i->check_types(global_scope, func, verbose);
    }

    for (auto i : else_body) {
        i->check_types(global_scope, func, verbose);
    }
}

void AST::IfStatement::reserve(AST::Program* prg) {
    cond->reserve(prg);

    for (auto i : body) {
        i->reserve(prg);
    }

    for (auto i : else_body) {
        i->reserve(prg);
    }
}

std::string AST::IfStatement::gen_code(Scope* scope, Function* func) {
    std::string output, fail_label, post_else_label;

    /* generate a label for when the condition is false */
    fail_label = func->make_label();

    std::string cond_type = cond->type(scope, func);

    /* first, get the value of the conditional expression */
    output += cond->gen_code(scope, func, true);

    /* if the condition fails, jump to the fail label */
    output += std::string("    ==0") + cond_type[0] + " " + fail_label + "\n";
    for (auto i : body) output += i->gen_code(scope, func);

    if (has_else) {
        post_else_label = func->make_label();
        output += "    goto " + post_else_label + "\n";
    }

    output += fail_label + ":";

    /* if there is an else block, we need another label after the else block */
    /* jump to it at the end of the initial body */

    if (has_else) {
        for (auto i : else_body) output += i->gen_code(scope, func);
        output += post_else_label + ":";
    }

    return output;
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

void AST::ForStatement::check_types(Scope* global_scope, Function* func, bool verbose) {
    if (cond) {
        std::string cond_type = cond->type(global_scope, func);

        if (cond_type != "int" && cond_type != "char" && cond_type != "float") {
            throw yy::parser::syntax_error(loc, "invalid condition type " + cond_type + " in 'for' statement");
        }
    }

    if (init) init->type(global_scope, func);
    if (next) next->type(global_scope, func);

    for (auto i : body) {
        i->check_types(global_scope, func, verbose);
    }
}

void AST::ForStatement::reserve(AST::Program* prg) {
    if (init) init->reserve(prg);
    if (cond) cond->reserve(prg);
    if (next) next->reserve(prg);

    for (auto i : body) {
        i->reserve(prg);
    }
}

std::string AST::ForStatement::gen_code(Scope* scope, Function* func) {
    std::string output, loop_label = func->make_label(), post_loop_label = func->make_label();

    if (init) output += init->gen_code(scope, func, false);
    output += loop_label + ":";

    if (cond) {
        std::string cond_type = cond->type(scope, func);
        output += cond->gen_code(scope, func, true);
        output += std::string("    ==0") + cond_type[0] + " " + post_loop_label + "\n";
    }

    std::string body_code;
    for (auto i : body) body_code += i->gen_code(scope, func);
    body_code = backpatch(body_code, "<PRELOOP>", loop_label);
    body_code = backpatch(body_code, "<POSTLOOP>", post_loop_label);

    output += body_code;

    if (next) {
        output += next->gen_code(scope, func, false);
    }

    output += "    goto " + loop_label + "\n";
    output += post_loop_label + ":";
    return output;
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

void AST::WhileStatement::check_types(Scope* global_scope, Function* func, bool verbose) {
    std::string cond_type = cond->type(global_scope, func);

    if (cond_type != "int" && cond_type != "char" && cond_type != "float") {
        throw yy::parser::syntax_error(loc, "invalid condition type " + cond_type + " in 'while' statement");
    }

    for (auto i : body) {
        i->check_types(global_scope, func, verbose);
    }
}

void AST::WhileStatement::reserve(AST::Program* prg) {
    cond->reserve(prg);

    for (auto i : body) {
        i->reserve(prg);
    }
}

std::string AST::WhileStatement::gen_code(Scope* scope, Function* func) {
    /* we only need a single label at the beginning of the loop,
     * and another one after the loop.
     * as we evaluate the conditional expression every time, we place the
     * label marker immediately before it */

    std::string loop_label = func->make_label(), post_loop_label = func->make_label(), output;
    std::string cond_type = cond->type(scope, func);

    /* generate body code early, so we can backpatch break/continue */
    std::string body_code;
    for (auto i : body) body_code += i->gen_code(scope, func);

    output += loop_label + ":";
    output += cond->gen_code(scope, func, true);
    output += std::string("    ==0") + cond_type[0] + " " + post_loop_label + "\n";
    
    /* perform any needed backpatching */
    body_code = backpatch(body_code, "<PRELOOP>", loop_label);
    body_code = backpatch(body_code, "<POSTLOOP>", post_loop_label);

    output += body_code;
    output += "    goto " + loop_label + "\n";
    output += post_loop_label + ":";

    return output;
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

void AST::DoWhileStatement::check_types(Scope* global_scope, Function* func, bool verbose) {
    std::string cond_type = cond->type(global_scope, func);

    if (cond_type != "int" && cond_type != "char" && cond_type != "float") {
        throw yy::parser::syntax_error(loc, "invalid condition type " + cond_type + " in 'while' statement");
    }

    for (auto i : body) {
        i->check_types(global_scope, func, verbose);
    }
}

void AST::DoWhileStatement::reserve(AST::Program* prg) {
    cond->reserve(prg);

    for (auto i : body) {
        i->reserve(prg);
    }
}

std::string AST::DoWhileStatement::gen_code(Scope* scope, Function* func) {
    /* very similar to WhileStatement, except evaluation of conditional
     * is moved after the body code */

    /* we actually need 3 labels, as continue; jumps before the conditional evaluation but after the code */

    std::string loop_label = func->make_label(), post_loop_label = func->make_label(), output;
    std::string pre_cond_label = func->make_label();
    std::string cond_type = cond->type(scope, func);

    /* generate body code early, so we can backpatch break/continue */
    std::string body_code;
    for (auto i : body) body_code += i->gen_code(scope, func);

    output += loop_label + ":";
    
    /* perform any needed backpatching */
    body_code = backpatch(body_code, "<PRELOOP>", pre_cond_label);
    body_code = backpatch(body_code, "<POSTLOOP>", post_loop_label);
    output += body_code;
    output += pre_cond_label + ":" + cond->gen_code(scope, func, true);
    output += std::string("    !=0") + cond_type[0] + " " + loop_label + "\n";
    output += post_loop_label + ":";

    return output;
}
