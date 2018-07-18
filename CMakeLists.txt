cmake_minimum_required(VERSION 3.8)
project(Assembler)

set(CMAKE_CXX_STANDARD 11)


set(SOURCE_FILES Source/Assembler.c Source/Stack.h Source/Commands.h Source/main.c Source/Cpu.h Source/Assembler.h)
add_executable(Assembler ${SOURCE_FILES})