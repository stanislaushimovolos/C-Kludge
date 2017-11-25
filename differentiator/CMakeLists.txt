cmake_minimum_required(VERSION 3.8)
project(differentiator)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Diff/main.cpp Tree_t/Tree.cpp Tree_t/Tree.h Diff/Diff.cpp Diff/Diff.h Fileworking/fileworking.h Diff/MathFunc.h)
add_executable(differentiator ${SOURCE_FILES} Tree_t/Tree.cpp Tree_t/Tree.h)