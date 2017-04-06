#include "Parser.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
  std::fstream f(argv[1], std::ios_base::in);
  std::stringstream s;
  std::string line;
  while (std::getline(f, line)) {
    s << line << "\n";
  }
  f.close();
  std::string input = s.str();

  Parser p(input.c_str(), input.size());
  Expression* e;
  try {
    e = p.parse();
  }
  catch (ParserException& ex) {
    std::cerr << ex.what() << std::endl;
    return 1;
  }

  if (e != nullptr) {
    std::cout << "Parsed expression: " << *e << std::endl;
  }
}

