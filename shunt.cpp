#include <iostream>
#include <fstream>
#include <vector>
#include <string>

enum TokenType {
  Number,          // 0..9 -0..9                123823.12312e+12, -10
  String,          // "                         "abcdef"
  Open,            // (                         (
  Close,           // )                         )
  OpenBracket,     // [                         [
  CloseBracket,    // ]                         ]
  OpenBrace,       // {                         {
  CloseBrace,      // }                         }
  Identifier,      // a..z A..Z _               myName_Max
  Function,        // a..z A..Z _ (             f(
  Operator         // !$%&'*+,-./:;<=>?@\^`|~   :=
};

std::vector<std::string> typeNames{"Num", "Str", "Op(", "Cl)", "Op[", "Cl]",
  "Op{", "Cl}", "Ide", "Fun", "Opr"};

std::vector<std::string> lines;

struct Token {
  TokenType type;
  std::string tok;
  Token(TokenType t, std::string const& s) : type(t), tok(s) {}
};

std::vector<Token> tokens;

void error(size_t& i, size_t& p, std::string const& message) {
  std::cerr << message << "\n" << lines[i] << "\n";
  for (size_t j = 0; j < p; ++j) {
    std::cerr << ' ';
  }
  std::cerr << "^" << std::endl;
  exit(1);
}

void scanNumber(size_t& i, size_t& p) {
  size_t q = p;
  while (p < lines[i].size() && lines[i][p] >= '0' && lines[i][p] <= '9') {
    ++p;
  }
  tokens.emplace_back(Number, lines[i].substr(q, p-q));
  --p;
}

void scanString(size_t& i, size_t& p) {
  std::string s;
  ++p;
  while (p < lines[i].size() && lines[i][p] != '"') {
    s.push_back(lines[i][p++]);
  }
  if (p >= lines[i].size()) {
    error(i, p, "String constant ended in end of line");
  }
  tokens.emplace_back(String, s);
}

bool isOp(char c) {
  return c == '!' || c == '$' || c == '%' || c == '&' || c == '\'' ||
         c == '*' || c == '+' || c == ',' || c == '-' || c == '.' ||
         c == '/' || c == ':' || c == ';' || c == '<' || c == '=' ||
         c == '>' || c == '?' || c == '@' || c == '\\' || c == '^' ||
         c == '`' || c == '|' || c == '~';
}

void scanOperator(size_t& i, size_t& p) {
  size_t q = p;
  while (p < lines[i].size() && isOp(lines[i][p])) {
    ++p;
  }
  tokens.emplace_back(Operator, lines[i].substr(q, p-q));
  --p;
}

bool isLetter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isWhite(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void skipWhite(size_t& i, size_t& p) {
  while (i < lines.size()) {
    while (p < lines[i].size()) {
      if (!isWhite(lines[i][p])) {
        return;
      }
      ++p;
    }
    ++i;
  }
}

void scanIdentifier(size_t& i, size_t& p) {
  size_t q = p;
  while (p < lines[i].size() && isLetter(lines[i][p])) {
    ++p;
  }
  std::string s = lines[i].substr(q, p-q);
  skipWhite(i, p);
  if (p < lines[i].size()) {
    if (lines[i][p] == '(') {
      tokens.emplace_back(Function, s);
      tokens.emplace_back(Open, "(");
    } else if (lines[i][p] == '[') {
      tokens.emplace_back(Function, s);
      tokens.emplace_back(OpenBracket, "[");
    } else if (lines[i][p] == '{') {
      tokens.emplace_back(Function, s);
      tokens.emplace_back(OpenBrace, "{");
    } else {
      --p;
    }
  } else {
    tokens.emplace_back(Identifier, s);
    --p;
  }
}

void tokenize() {
  for (size_t i = 0; i < lines.size(); ++i) {
    std::string& line(lines[i]);
    for (size_t p = 0; p < line.size(); ++p) {
      switch (line[p]) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          scanNumber(i, p);
          break;
        case '(':
          tokens.emplace_back(Open, line.substr(p, 1));
          break;
        case '[':
          tokens.emplace_back(OpenBracket, line.substr(p, 1));
          break;
        case '{':
          tokens.emplace_back(OpenBrace, line.substr(p, 1));
          break;
        case ')':
          tokens.emplace_back(Close, line.substr(p, 1));
          break;
        case ']':
          tokens.emplace_back(CloseBracket, line.substr(p, 1));
          break;
        case '}':
          tokens.emplace_back(CloseBrace, line.substr(p, 1));
          break;
        case '"':
          scanString(i, p);
          break;
        case '#':   // a comment
          p = line.size();
          break;
        default:
          char c = line[p];
          if (isOp(c)) {
            scanOperator(i, p);
          } else if (isLetter(c)) {
            scanIdentifier(i, p);
          } else if (!isWhite(c)) {
            error(i, p, "Unexpected character:");
          }
      }
    }
  }
}

int main(int argc, char* argv[]) {
  std::fstream f(argv[1], std::ios_base::in);
  std::string line;
  while (std::getline(f, line)) {
    lines.push_back(line);
  }
  f.close();

  tokenize();
  for (auto const& t : tokens) {
    std::cout << typeNames[t.type] << " " << t.tok << "\n";
  }
}
