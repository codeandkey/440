Justin Stanley
COMS 440 Spring 2019
Project part 4 : code generation

For implementation details please see developers.pdf.
Note: for this part I decided to rewrite all of the previous stages as well as the code was getting very messy and contrived.
I think that reimplementing the compiler in C++ greatly benefited the organization of the project and made implementing this part much easier.

Rubric areas:
    <Ease of building the executable>
        Executing `make` should build the compiler (builds on g++ 8.2.1)
    <Developer documentation>
        (See developers.pdf)
    <Non-instruction output>
        The compiler generates correct non-instruction output.
        The three headers (constants, global count, function count) are output in AST::Program::generate_ir().
        The function headers (name, number, params, locals, return) are output in AST::Function::gen_code().
    <Constants, in expressions>
        The compiler outputs the correct code for constants in expressions.
        Every constant value is stored in .CONSTANTS, although integer and character constants do not require it (pushv can be used directly instead)
        For code generation see AST::{Int,Char,Str,Real}Const::gen_code()
    <Variables unchanging in expressions>
        The compiler generates the correct code for unchanging variables.
        (see AST::IdentifierExpression::gen_code)
    <return statements>
        The compiler generates the correct code for return statements.
        (see AST::ReturnStatement::gen_code)
    <Function calls (including parameter passing)>
        The compiler generates correct code for calling functions, with parameters and return values.
        (see AST::CallExpression::gen_code)
    <Builtin functions>
        The compiler correctly declares getchar/putchar as builtin functions with respective numbers 0,1.
        The "digit sum" program works from Part4.pdf.
        (see AST::Program::Program)
    <Expression operations>
        The compiler generates code for all of the operators (unary '-', '~', '+', '-', '*', '/', '%', '|', '&'), as well as casting.
        (see AST::UnaryOpExpression::gen_code, AST::BinaryOpExpression::gen_code, AST::CastExpression::gen_code)
    <Assignments to variables>
        The compiler generates correct code for variable assignments.
        (see AST::AssignmentExpression::gen_code and AST::LValue::gen_store_code)
    <Update assignments to variables>
        The compiler generates correct code for update assignments to variables.
        (see AST::AssignmentExpression::gen_code, AST::LValue::gen_retrieve_code, AST::LValue::gen_store_code)
    <Incremement, decrement of variables>
        The compiler generates correct code for incrementing and decrementing variables.
        (see AST::IncDecExpression::gen_code, AST::LValue::gen_retrieve_code, AST::LValue::gen_store_code)
    <String literals in constants>
        The compiler correctly generates character arrays for strings in constant data.
        (see AST::Program::make_const_str)
    <Array variables, string literals passed as parameters>
        The compiler generates correct code for passing all array types as parameters.
        (see AST::CallExpression::gen_code)
    <Array elements unchanging in expressions>
        The compiler generates correct code for array indexing.
        (see AST::IndexExpression::gen_code)
    <Assignments to arrays>
        The compiler generates correct code for assignments to arrays.
        (see AST::AssignmentExpression::gen_code, AST::LValue::gen_store_code)
    <Update assignment to arrays>
        The compiler generates correct code for update assignments to arrays.
        (see AST::AssignmentExpression::gen_code, AST::LValue::gen_retrieve_code, AST::LValue::gen_store_code)
    <Increment, decrement of arrays>
        The compiler generates correct code for incrementing and decrementing array values.
        (see AST::IncDecExpression::gen_code, AST::LValue::gen_retrieve_code, AST::LValue::gen_store_code)
