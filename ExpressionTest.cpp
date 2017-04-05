#include "Expression.h"
#include <iostream>

int main(int argc, char* argv[]) {
  Expression e1(12L);
  std::cout << "A number: " << e1 << std::endl;
  Expression e2("abc");
  std::cout << "A string: " << e2 << std::endl;
  Expression e3(123.456);
  std::cout << "A double: " << e3 << std::endl;
  Expression e4(ExprType::IDEN, ExprBound::NONE, "abc");
  std::cout << "An identifier: " << e4 << std::endl;
  Expression e5(ExprType::OPER, ExprBound::ROUND, ",");
  e5.push(new Expression(1L));
  e5.push(new Expression("xyz"));
  e5.push(new Expression(ExprType::IDEN, ExprBound::NONE, "function"));
  std::cout << "A list: " << e5 << std::endl;
  Expression e6(ExprType::FUNC, ExprBound::NONE, "f");
  Expression* e7 = new Expression(ExprType::OPER, ExprBound::BRACKET, ",");
  e7->push(new Expression(123L));
  e7->push(new Expression("asdgaszdgzasd"));
  e6.push(e7);
  std::cout << "A function: " << e6 << std::endl;
  return 0;
}
