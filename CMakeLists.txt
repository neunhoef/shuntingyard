cmake_minimum_required (VERSION 3.5)

project (Shunt CXX)

add_executable(shunt shunt.cpp)
set_property(TARGET shunt PROPERTY CXX_STANDARD 11)
set_property(TARGET shunt PROPERTY CXX_STANDARD_REQUIRED ON)
