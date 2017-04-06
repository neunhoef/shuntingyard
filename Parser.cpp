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
  _tokens.emplace_back(TokenType::Number, q, _pos - q, _line);
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
  _tokens.emplace_back(TokenType::String, q, _pos - q, _line);
  more = advance();
  return more;
}

bool Parser::scanOperator() {
  size_t q = _pos;
  bool more = true;
  while (more && isOp(_start[_pos])) {
    more = advance();
  }
  _tokens.emplace_back(TokenType::Operator, q, _pos - q, _line);
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
    _tokens.emplace_back(TokenType::Identifier, q, _pos - q, _line);
    return false;
  }
  size_t q2 = _pos;
  size_t line2 = _line;
  more = skipWhite();
  if (more) {
    if (_start[_pos] == '(' || _start[_pos] == '[' || _start[_pos] == '{') {
      _tokens.emplace_back(TokenType::Function, q, q2 - q, line2);
      _tokens.emplace_back(TokenType::Open, _pos, 1, _line);
      more = advance();
    } else {
      _tokens.emplace_back(TokenType::Identifier, q, q2 - q, line2);
    }
  } else {
    _tokens.emplace_back(TokenType::Identifier, q, q2 - q, line2);
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
        _tokens.emplace_back(TokenType::Open, _pos, 1, _line);
        more = advance();
        break;
      case ')':
      case ']':
      case '}':
        _tokens.emplace_back(TokenType::Close, _pos, 1, _line);
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

void Parser::parseError(std::string const& msg, Token const& t) {
  std::stringstream m;
  m << "Parse error: line " << t.line << ":" << msg << "\n";
  char const* p = _start + t.start;
  while (p > _start && *p != '\n') {
    --p;
  }
  if (p > _start) {
    ++p;
  }
  char const* q = _start + t.start;
  while (q - p < _size && *q != '\n') {
    ++q;
  }
  m << std::string(p, q-p) << "\n";
  for (size_t i = 0; i < (_start + t.start - p); i++) {
    m << ' ';
  }
  m << "^\n";
  throw ParserException(m.str());
}

Expression* Parser::parseInternal() {
  std::vector<Expression*> stack;

  // The following takes one or more tokens from the _opStack and "executes"
  // it in the sense that an Expression* is contructed incorporation the
  // right things from the stack. Only call when there is an operation
  // or a function on top of the _opStack!
  auto execute = [&]() -> void {
    Token& tos = _opStack.back();

    if (tos.type == TokenType::Operator) {
      // Drill down until we hit the bottom or another operator:
      size_t pos = _opStack.size()-1;
      while (pos > 0 && tokensEqual(_opStack[pos-1], tos)) {
        --pos;
      }
      Expression* e;
      if (pos > 0 && _opStack[pos-1].type == TokenType::Open) {
        e = new Expression(ExprType::OPER,
            Expression::boundFromChar(_start[_opStack[pos-1].start]),
            toString(tos));
      } else {
        e = new Expression(ExprType::OPER, ExprBound::NONE, toString(tos));
      }
      size_t nrItems = _opStack.size() - pos + 1;
      for (size_t i = stack.size() - nrItems; i < stack.size(); ++i) {
        e->push(stack[i]);
      }
      for (size_t i = 0; i < nrItems; ++i) {
        stack.pop_back();
      }
      stack.push_back(e);
      _opStack.pop_back();
    } else {  // tos.type == TokenType::Function
      // Drill down in the stack until we find our FUNC entry:
      size_t pos = stack.size()-1;
      while (pos > 0 && stack[pos]->type() != ExprType::FUNC) {
        --pos;
      }
      size_t nrArgs = stack.size() - 1 - pos;
      Expression* e = stack[pos];
      for (size_t i = pos + 1; i <= pos + nrArgs; ++i) {
        e->push(stack[i]);
      }
      for (size_t i = 0; i < nrArgs; ++i) {
        stack.pop_back();
      }
      _opStack.pop_back();
    }
  };

  for (size_t i = 0; i < _tokens.size(); ++i) {
    Token& t = _tokens[i];
    switch (t.type) {
      case TokenType::Number: {
        std::string n(toString(t));
        stack.push_back(new Expression(static_cast<int64_t>(std::stol(n))));
        break;
      }
      case TokenType::String: {
        stack.push_back(new Expression(toString(t)));
        break;
      }
      case TokenType::Identifier: {
        stack.push_back(new Expression(ExprType::IDEN, ExprBound::NONE,
                                       toString(t)));
        break;
      }
      case TokenType::Function: {
        _opStack.push_back(t);
        stack.push_back(new Expression(ExprType::FUNC, ExprBound::NONE,
                                       toString(t)));
        break;
      }
      case TokenType::Open: {
        _opStack.push_back(t);
        break;
      }
      case TokenType::Operator: {
        std::string newOp = toString(t);
        uint32_t precNew = precedence(newOp);
        while (_opStack.size() > 0 &&
               _opStack.back().type == TokenType::Operator) {
          Token& tos = _opStack.back();
          std::string oldOp = toString(tos);
          uint32_t precOld = precedence(oldOp);
          if (precOld < precNew || (oldOp == newOp)) {
            break;
          }
          execute();
        }
        _opStack.push_back(t);
        break;
      }
      case TokenType::Close: {
        bool sawArg = false;
        while (_opStack.size() > 0 && _opStack.back().type != TokenType::Open) {
          sawArg = true;
          execute();
        }
        if (_opStack.size() == 0) {
          parseError("found unmatched closing bracket", t);
        }
        Token& tos = _opStack.back();
        if ((_start[t.start] == ')' && _start[tos.start] != '(') ||
            (_start[t.start] == ']' && _start[tos.start] != '[') ||
            (_start[t.start] == '}' && _start[tos.start] != '{')) {
          parseError("found mismatched closing bracket", t);
        }
        _opStack.pop_back();   // the opening bracket
        // If we get here, there must have been a token before the current one!
        if (_tokens[i-1].type == TokenType::Open) {
          // Case of no arguments in bracket expression, we create
          // an artificial one and put it on top of the stack:
          Expression* e = new Expression(ExprType::OPER,
              Expression::boundFromChar(_start[t.start]), ",");
          stack.push_back(e);
        } else if (stack.size() > 0 &&
                   stack.back()->bound() == ExprBound::NONE) {
          // If there is at least one argument on the stack, make sure that
          // it has the right bound set:
          stack.back()->setBound(Expression::boundFromChar(_start[t.start]));
        }
        if (_opStack.size() > 0 &&
            _opStack.back().type == TokenType::Function) {
          if (i+1 >= _tokens.size() || _tokens[i+1].type != TokenType::Open) {
            if (stack.back()->bound() == ExprBound::NONE) {
              stack.back()->setBound(
                  Expression::boundFromChar(_start[t.start]));
            }
            execute();
          }
        }
        break;
      }
    }
  }
  while (_opStack.size() > 0) {
    if (_opStack.back().type == TokenType::Open) {
      parseError("found unmatched opening bracket", _opStack.back());
    }
    execute();
  }
  if (stack.size() == 0) {
    return nullptr;
  } else if (stack.size() == 1) {
    return stack[0];
  } else {
    Expression* res = new Expression(ExprType::OPER, ExprBound::NONE, " ");
    for (Expression* e : stack) {
      res->push(e);
    }
    return res;
  }
}

