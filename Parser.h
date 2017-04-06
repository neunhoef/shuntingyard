#include <cstdint>
#include <vector>
#include <string>
#include <exception>
#include <unordered_map>
#include <cstring>

#include "Expression.h"

class ParserException : std::exception {
  std::string _msg;
 public:
  ParserException(std::string const& msg) : _msg(msg) { }
  char const* what() const noexcept override {
    return _msg.c_str();
  }
};

class Parser {

  enum class TokenType : uint32_t {
    Number = 0,          // 0..9 -0..9                123823.12312e+12, -10
    String = 1,          // "                         "abcdef"
    Identifier = 2,      // a..z A..Z _               myName_Max
    Open = 3,            // (                         (
    Close = 4,           // )                         )
    Operator = 5,        // !$%&'*+,-./:;<=>?@\^`|~   :=
    Function = 6         // a..z A..Z _ (             f(
  };

  static std::vector<std::string> typeNames;
  static std::unordered_map<std::string, uint32_t> operatorPrecedence;

  struct Token {
    TokenType type;
    uint32_t start;   // Index in _input of parser
    uint32_t size;    // Size in bytes

    Token(TokenType t, size_t st, size_t si)
      : type(t), start(static_cast<uint32_t>(st)),
                 size(static_cast<uint32_t>(si)) {}
  };

  std::string toString(Token const& t) {
    return std::string(_start + t.start, t.size);
  }

  uint32_t precedence(std::string const& op) {
    auto it = operatorPrecedence.find(op);
    if (it != operatorPrecedence.end()) {
      return it->second;
    }
    return 6;
  }

  bool tokensEqual(Token const& left, Token const& right) {
    if (left.type != right.type || left.size != right.size) {
      return false;
    }
    return strncmp(_start + left.start, _start + right.start, left.size) == 0;
  }
  
  char const* _start;
  size_t _size;
  size_t _pos;
  size_t _line;
  size_t _col;

  bool advance() {
    if (_start[_pos] == '\n') {
      ++_line;
      _col = 0;
    } else {
      ++_col;
    }
    ++_pos;
    return _pos < _size;
  }

  std::vector<Token> _tokens;
  std::vector<Token> _output;   // this is in reverse polish notation
  std::vector<Token> _opStack;

  bool isOp(char c) {
    return c == '!' || c == '$' || c == '%' || c == '&' || c == '\'' ||
           c == '*' || c == '+' || c == ',' || c == '-' || c == '.' ||
           c == '/' || c == ':' || c == ';' || c == '<' || c == '=' ||
           c == '>' || c == '?' || c == '@' || c == '\\' || c == '^' ||
           c == '`' || c == '|' || c == '~';
  }

  bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }

  bool isWhite(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
  }

  void tokenizerError(std::string const& message);
  bool scanNumber();
  bool scanString();
  bool scanOperator();
  bool skipWhite();
  bool  scanIdentifier();
  void parseError();

  void tokenize();

  Expression* parseInternal();

 public:
  Parser(char const* start, size_t size)
    : _start(start), _size(size), _pos(0), _line(1), _col(0) {
  }

  Expression* parse() {
    tokenize();
    showTokens(std::cout);
    Expression* e = parseInternal();
    _tokens.clear();
    showOutput(std::cout);
    _output.clear();
    return e;
  }

  void showTokens(std::ostream& out) {
    out << "Tokenizer found:\n";
    for (auto const& t : _tokens) {
      out << typeNames[(uint32_t) t.type] << " "
          << std::string(_start + t.start, t.size) << "\n";
    }
    out << std::endl;
  }

  void showOutput(std::ostream& out) {
    out << "Parser produced:\n";
    for (auto const& t : _output) {
      out << std::string(_start + t.start, t.size) << "\n";
    }
    out << std::endl;
  }
};
