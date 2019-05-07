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
    <Expressions and function calls>
        All of the functionality from the previous part is still present.

    All statements and operators are implemented, with correct short-circuiting behavior.
    I'm not sure what the short-circuit behavior for comparisons/boolean assignments should be, but it is correctly implemented for '||' and '&&' operators.

    The short-circuit implementation is in the expression code generation, so it will should work in any statement (if, for, ...).
