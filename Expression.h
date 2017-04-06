#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

enum class ExprType : uint32_t {
 /* the leaves: */
  INT64,
  DOUBLE,
  STRING,
  IDEN,
 /* the nodes: */
  OPER,
  FUNC
};

enum class ExprBound : uint32_t {
  NONE,
  ROUND,
  BRACKET,
  BRACE
};

class Expression {
  ExprType _type;         // indicates type of Expression
  ExprBound _bound;       // only != NONE for ExprType::OPER and ExprType::FUNC
  union {
    int64_t _i;           // for ExprType::INT64
    double _d;            // for ExprType::DOUBLE
  };
  std::string _s;         // for all, empty for the numerical types
  std::vector<Expression*>* _children;  // nullptr for leafs

 public:
  Expression(int64_t i)
    : _type(ExprType::INT64), _bound(ExprBound::NONE),
      _i(i), _children(nullptr) {}
  Expression(double d)
    : _type(ExprType::DOUBLE), _bound(ExprBound::NONE),
      _d(d), _children(nullptr) {}
  Expression(std::string const& s)
    : _type(ExprType::STRING), _bound(ExprBound::NONE),
      _i(0), _s(s), _children(nullptr) {}
  Expression(std::string&& s)
    : _type(ExprType::STRING), _bound(ExprBound::NONE),
      _i(0), _s(s), _children(nullptr) {}
  Expression(ExprType ty, ExprBound bo, std::string const& s)
    : _type(ty), _bound(bo), _i(0), _s(s) {
    _children = new std::vector<Expression*>();
  }

  ~Expression() {
    if (_children != nullptr) {
      for (Expression* p : *_children) {
        delete p;
      }
      delete _children;
    }
  }

  ExprType type() {
    return _type;
  }

  ExprBound bound() {
    return _bound;
  }

  void push(Expression* e) {
    _children->push_back(e);
  }

  void stringify(std::ostream& out) const {
    bool first;
    switch (_type) {
      case ExprType::INT64:
        out << _i;
        break;
      case ExprType::DOUBLE:
        out << _d;
        break;
      case ExprType::STRING:
        out << '"';
        // FIXME: add proper quoting
        out << _s;
        out << '"';
        break;
      case ExprType::IDEN:
        out << _s;
        break;
      case ExprType::OPER:
        switch (_bound) {
          case ExprBound::ROUND:
            out << '(';
            break;
          case ExprBound::BRACKET:
            out << '[';
            break;
          case ExprBound::BRACE:
            out << '{';
            break;
          default:
            break;
        }
        first = true;
        for (auto const* e : *_children) {
          if (!first) {
            out << _s;
          }
          first = false;
          e->stringify(out);
        }
        switch (_bound) {
          case ExprBound::ROUND:
            out << ')';
            break;
          case ExprBound::BRACKET:
            out << ']';
            break;
          case ExprBound::BRACE:
            out << '}';
            break;
          default:
            break;
        }
        break;
      case ExprType::FUNC:
        out << _s;
        for (auto const* e : *_children) {
          e->stringify(out);
        }
        break;
    }
  }
};

namespace std {
  std::ostream& operator<<(std::ostream& out, Expression const& e);
}
