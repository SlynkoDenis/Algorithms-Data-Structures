cmake_minimum_required(VERSION 3.15)
project(DijkstraAlgorithm)

set(CMAKE_CXX_STANDARD 17)

add_executable(DijkstraAlgorithm main.cpp ../../DataStructures/binary_heap/binary_heap.h ../../DataStructures/graph/graph.h)
