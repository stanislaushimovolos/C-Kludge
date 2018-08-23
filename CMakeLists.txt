cmake_minimum_required(VERSION 3.8)
project(Assembler)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=RELEASE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=DEF")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=ASM")

set(SOURCE_FILES Source/Cpu/stack.h Source/Commands.h Source/Cpu/cpu.h Source/Cpu/main.cpp Source/Cpu/vector.h)
#set(SOURCE_FILES Source/Assembler/Assembler.h Source/Assembler/Assembler.c Source/Assembler/main.c  Source/Commands.h)
add_executable(Assembler ${SOURCE_FILES})