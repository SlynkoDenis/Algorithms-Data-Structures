cmake_minimum_required(VERSION 3.17)
project(red_black_tree)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror)

include_directories(.)

add_executable(red_black_tree
        main.cpp
        red_black_tree.h)
