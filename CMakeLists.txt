cmake_minimum_required(VERSION 3.8)
project(Assembler)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=RELEASE")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=DEF")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=ASM")
set(SOURCE_FILES Asm/Assembler.cpp Stack/Stack.h CommonFiles/commandsAsm&CPU.h CommonFiles/fileworking.h)
add_executable(Assembler ${SOURCE_FILES})