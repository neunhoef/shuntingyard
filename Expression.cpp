#include "Expression.h"

namespace std {
  std::ostream& operator<<(std::ostream& out, Expression const& e) {
    e.stringify(out);
    return out;
  }
}
