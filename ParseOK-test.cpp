#include <string>
#include "catch.hpp"
#include "Parser.h"

TEST_CASE("ParseOK", "[parseok]") {

SECTION("number") {
  std::string number = R"(123)";
  try {
    Parser p(number.c_str(), number.size());
    Expression* e = p.parse();
    CHECK(nullptr != e);
    CHECK(ExprType::INT64 == e->type());
    CHECK(123 == e->getInt64());
    CHECK(nullptr == e->getChildren());
  }
  catch (ParserException const& ex) {
    CHECK(false);
  }
}

}
