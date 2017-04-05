cmake_minimum_required (VERSION 3.5)

project (Shunt CXX)

add_executable(shunt shunt.cpp)
set_property(TARGET shunt PROPERTY CXX_STANDARD 11)
set_property(TARGET shunt PROPERTY CXX_STANDARD_REQUIRED ON)

add_executable(ExpressionTest ExpressionTest.cpp Expression.cpp)
set_property(TARGET ExpressionTest PROPERTY CXX_STANDARD 11)
set_property(TARGET ExpressionTest PROPERTY CXX_STANDARD_REQUIRED ON)

add_executable(ParserTest ParserTest.cpp Expression.cpp Parser.cpp)
set_property(TARGET ParserTest PROPERTY CXX_STANDARD 11)
set_property(TARGET ParserTest PROPERTY CXX_STANDARD_REQUIRED ON)
