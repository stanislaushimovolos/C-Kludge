cmake_minimum_required(VERSION 3.8)
project(Assembler)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=RELEASE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=DEF")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=ASM")

set(SOURCE_FILES Source/Assembler/Assembler.c Source/Cpu/Stack.h Source/Commands.h Source/Assembler/main.c Source/Cpu/Cpu.h Source/Assembler/Assembler.h)
add_executable(Assembler ${SOURCE_FILES})