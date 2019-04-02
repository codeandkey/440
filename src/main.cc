#include "driver.hh"

#define MODE_LEXER 1
#define MODE_PARSE 2

int usage(char** argv);

bool opt_verbose = false;

int main(int argc, char** argv) {
    int i, mode = 0;
    for (i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "-l" || arg == "--lex")     { mode |= MODE_LEXER; continue; }
        if (arg == "-p" || arg == "--parse")   { mode |= MODE_PARSE; continue; }
        if (arg == "-v" || arg == "--verbose") { opt_verbose = true; continue; }
        if (arg == "--")                       { ++i; break; }

        if (arg[0] == '-') {
            /* catch invalid options */
            std::cerr << "error: unknown option " << argv[i] << "\n";
            return usage(argv);
        }

        break;
    }

    switch (mode) {
    case MODE_LEXER:
        for (; i < argc; ++i) {
            if (driver().scan(argv[i])) return 1;
        }
        return 0;
    case MODE_PARSE:
        break;
    default:
        std::cerr << "error: invalid execution mode. cannot continue.\n";
        return usage(argv);
    }
}

int usage(char** argv) {
    std::cout << "usage:\n\t" << *argv << " [-v] {-l,-p} <filename> (...)\n";
    return EXIT_FAILURE;
}
