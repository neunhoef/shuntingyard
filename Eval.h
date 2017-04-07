#include "Expression.h"

#include <string>

// Here is the complete set of evaluation rules:
//
// (1) Leafs evaluate to themselves, unless they are identifiers.
// (2) Identifiers are looked up in the name space of the Eval object
//     and then in the parent name space and so on until there is no
//     more parent, and then the _system name space is taken. If found
//     the result is the expression found, otherwise the result is nil.
// (3) Operator and function expressions perform a lookup in the function
//     name space.
//
// Here are some special functions that are useful:

class Eval {
  Eval* _parent;
  static Eval* _system;
  std::unordered_map<std::string, Expression*> _symbols;

 public:
  Eval(Eval* parent) : _parent(parent) {
  }

  Expression* eval(Expression* e);

  void bind(std::string const& key, Expression* value);
  void unbind(std::string const& key);
  Expression* get(std::string const& key);
}
