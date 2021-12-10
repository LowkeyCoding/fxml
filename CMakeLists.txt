cmake_minimum_required(VERSION 3.21)
project(test C)

set(CMAKE_C_STANDARD 99)

add_executable(tests tests.c libs/CuTest.c)
add_executable(demo sxml_demo.c)