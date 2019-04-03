#include "constants.hh"

AST::IntConst::IntConst(location loc, int n) : Expression(loc), n(n) {}
void AST::IntConst::write() { std::cout << "<IntConst n=" << n << ">\n"; }
std::string AST::IntConst::type() { return "int"; }

AST::RealConst::RealConst(location loc, double n) : Expression(loc), n(n) {}
void AST::RealConst::write() { std::cout << "<RealConst n=" << n << ">\n"; }
std::string AST::RealConst::type() { return "float"; }

AST::StrConst::StrConst(location loc, std::string val) : Expression(loc), val(val) {}
void AST::StrConst::write() { std::cout << "<StrConst val=\"" << val << "\">\n"; }
std::string AST::StrConst::type() { return "char[]"; }

AST::CharConst::CharConst(location loc, char val) : Expression(loc), val(val) {}
void AST::CharConst::write() { std::cout << "<CharConst val='" << val << "'>\n"; }
std::string AST::CharConst::type() { return "char"; }
