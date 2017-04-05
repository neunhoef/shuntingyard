#include "Parser.h"

#include <iostream>
#include <sstream>

std::vector<std::string> Parser::typeNames
  {"Num", "Str", "Idn", "Opn", "Cls", "Opr", "Fun"};

std::unordered_map<std::string, uint32_t> Parser::operatorPrecedence
  {{";", 0}, {",", 1}, {":=", 2}, {":", 3}, {"||", 4}, {"&&", 4},
   {"<", 5}, {">", 5}, {"<=", 5}, {">=", 5}, {"=", 5}, {"!=", 5},
   {"..", 7}, {"+", 8}, {"-", 8}, {"*", 9}, {"/", 9}, {"%", 9}, {"^", 10},
   {".", 11}};

void Parser::tokenizerError(std::string const& message) {
  std::ostringstream out;
  size_t lineStart = _pos - _col;
  size_t saveCol = _col;
  bool more = _pos < _size;
  while (more && _start[_pos] != '\n') {
    more = advance();
  }
  size_t lineEnd = _pos;
  std::string line(_start + lineStart, lineEnd - lineStart);
  out << message << "\n" << line << "\n";
  for (size_t j = 0; j < saveCol; ++j) {
    out << ' ';
  }
  out << "^" << std::endl;
  throw ParserException(out.str());
}

bool Parser::scanNumber() {
  size_t q = _pos;
  bool more = true;
  while (more && _start[_pos] >= '0' && _start[_pos] <= '9') {
    more = advance();
  }
  _tokens.emplace_back(TokenType::Number, q, _pos - q);
  return more;
}

bool Parser::scanString() {
  bool more = advance();
  size_t q = _pos;
  while (more && _start[_pos] != '"') {
    if (_start[_pos] == '\n') {
      tokenizerError("String constant ended in end of line");
    }
    more = advance();
  }
  if (!more) {
    tokenizerError("String constant ended in end of file");
  }
  _tokens.emplace_back(TokenType::String, q, _pos - q);
  more = advance();
  return more;
}

bool Parser::scanOperator() {
  size_t q = _pos;
  bool more = true;
  while (more && isOp(_start[_pos])) {
    more = advance();
  }
  _tokens.emplace_back(TokenType::Operator, q, _pos - q);
  return more;
}

bool Parser::skipWhite() {
  bool more = true;
  while (more && isWhite(_start[_pos])) {
    more = advance();
  }
  return more;
}

bool Parser::scanIdentifier() {
  size_t q = _pos;
  bool more = advance();
  while (more && (isLetter(_start[_pos]) ||
                  (_start[_pos] >= '0' && _start[_pos] <= '9'))) {
    more = advance();
  }
  if (!more) {
    _tokens.emplace_back(TokenType::Identifier, q, _pos - q);
    return false;
  }
  size_t q2 = _pos;
  more = skipWhite();
  if (more) {
    if (_start[_pos] == '(' || _start[_pos] == '[' || _start[_pos] == '{') {
      _tokens.emplace_back(TokenType::Function, q, q2 - q);
      _tokens.emplace_back(TokenType::Open, _pos, 1);
      more = advance();
    } else {
      _tokens.emplace_back(TokenType::Identifier, q, q2 - q);
    }
  } else {
    _tokens.emplace_back(TokenType::Identifier, q, q2 - q);
  }
  return more;
}

void Parser::tokenize() {
  bool more = _size > 0;
  while (more) {
    switch (_start[_pos]) {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        more = scanNumber();
        break;
      case '(':
      case '[':
      case '{':
        _tokens.emplace_back(TokenType::Open, _pos, 1);
        more = advance();
        break;
      case ')':
      case ']':
      case '}':
        _tokens.emplace_back(TokenType::Close, _pos, 1);
        more = advance();
        break;
      case '"':
        more = scanString();
        break;
      case '#':   // a comment
        more = advance();
        while (more && _start[_pos] != '\n') {
          more = advance();
        }
        if (more) {
          more = advance();
        }
        break;
      default:
        if (isOp(_start[_pos])) {
          more = scanOperator();
        } else if (isLetter(_start[_pos])) {
          more = scanIdentifier();
        } else if (isWhite(_start[_pos])) {
          more = skipWhite();
        } else {
          std::string message = "Unexpected character: ";
          message.push_back(_start[_pos]);
          tokenizerError(message);
        }
    }
  }
}

void Parser::parseError() {
  throw ParserException("Parse error");
}

void Parser::parseInternal() {
  std::string newOp;
  uint32_t precNew;
  std::string oldOp;
  uint32_t precOld;
  Token* tos;

  for (size_t i = 0; i < _tokens.size(); ++i) {
    Token& t = _tokens[i];
    switch (t.type) {
      case TokenType::Number:
      case TokenType::String: _output.push_back(t); break;
      case TokenType::Function: _opStack.push_back(t); break;
      case TokenType::Open: _opStack.push_back(t); break;
      case TokenType::Operator:
        newOp = toString(t);
        precNew = precedence(newOp);
        while (_opStack.size() > 0 &&
               _opStack.back().type == TokenType::Operator) {
          tos = &_opStack.back();
          oldOp = toString(*tos);
          precOld = precedence(oldOp);
          if (precOld < precNew || (oldOp == newOp)) {
            break;
          }
          _output.push_back(_opStack.back());
          _opStack.pop_back();
        }
        _opStack.push_back(t);
        break;
      case TokenType::Close:
        while (_opStack.size() > 0 && _opStack.back().type != TokenType::Open) {
          _output.push_back(_opStack.back());
          _opStack.pop_back();
        }
        if (_opStack.size() == 0) {
          parseError();
        }
        tos = &_opStack.back();
        if ((_start[t.start] == ')' && _start[tos->start] != '(') ||
            (_start[t.start] == ']' && _start[tos->start] != '[') ||
            (_start[t.start] == '}' && _start[tos->start] != '{')) {
          parseError();
        }
        _opStack.pop_back();   // the opening bracket
        if (_opStack.size() > 0 && _opStack.back().type == TokenType::Function) {
          if (i+1 >= _tokens.size() || _tokens[i+1].type != TokenType::Open) {
            _output.push_back(_opStack.back());
            _opStack.pop_back();
          }
        }
        break;
      default:
        parseError();
        break;
    }
  }
  while (_opStack.size() > 0) {
    if (_opStack.back().type == TokenType::Open ||
        _opStack.back().type == TokenType::Close) {
      parseError();
    }
    _output.push_back(_opStack.back());
    _opStack.pop_back();
  }
}

Expression* Parser::makeExpression() {
  return nullptr;
}
