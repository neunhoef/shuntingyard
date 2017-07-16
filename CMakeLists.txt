cmake_minimum_required (VERSION 3.5)

project (Shunt CXX C)

include_directories("include")

add_executable(shunt shunt.cpp)
set_property(TARGET shunt PROPERTY CXX_STANDARD 11)
set_property(TARGET shunt PROPERTY CXX_STANDARD_REQUIRED ON)

add_executable(ExpressionTest ExpressionTest.cpp Expression.cpp)
set_property(TARGET ExpressionTest PROPERTY CXX_STANDARD 11)
set_property(TARGET ExpressionTest PROPERTY CXX_STANDARD_REQUIRED ON)

add_executable(ParserTest ParserTest.cpp Expression.cpp Parser.cpp)
set_property(TARGET ParserTest PROPERTY CXX_STANDARD 11)
set_property(TARGET ParserTest PROPERTY CXX_STANDARD_REQUIRED ON)

add_executable(UnitTests UnitTests.cpp ParserOK-test.cpp Expression.cpp Parser.cpp)
set_property(TARGET UnitTests PROPERTY CXX_STANDARD 11)
set_property(TARGET UnitTests PROPERTY CXX_STANDARD_REQUIRED ON)

add_executable(vectortest vectortest.c vector.c)
set_property(TARGET vectortest PROPERTY C_STANDARD 11)
set_property(TARGET vectortest PROPERTY C_STANDARD_REQUIRED ON)
