cmake_minimum_required(VERSION 3.8)
project(Assembler)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=RELEASE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=DEF")
set(SOURCE_FILES main.cpp Stack.h commands.h)
add_executable(Assembler ${SOURCE_FILES})