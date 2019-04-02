#include "driver.hh"
#include "util.hh"

extern char* yytext;

driver::driver() : trace_parsing(false), trace_scanning(false) {
    variables["one"] = 1;
}

int driver::parse(const std::string& f) {
    file = f;
    location.initialize(&file);
    scan_begin();
    yy::parser parse(*this);
    parse.set_debug_level(trace_parsing);
    int res = parse();
    scan_end();
    return res;
}

int driver::scan(const std::string& f) {
    file = f;
    location.initialize(&file);
    scan_begin();

    try {
        while (true) {
            auto tok = yylex(*this);
            auto loc = tok.location;

            if (tok.type == yy::parser::token::TOK_END) break;
            std::cout << "File " << *(loc.begin.filename) << " Line " << loc.begin.line << " Token ";
            std::cout << util::symbol_type_name(tok) << " Text '" << yytext << "'\n";
        }
    } catch (yy::parser::syntax_error& e) {
        std::cerr << "Error in " << *(e.location.begin.filename) << " line " << e.location.begin.line << ":\n\t";
        std::cerr << e.what() << "\n";
        scan_end();
        return -1;
    }

    scan_end();
    return 0;
}
