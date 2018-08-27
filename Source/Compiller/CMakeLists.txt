cmake_minimum_required(VERSION 3.10)
project(LanguageRef)

set(CMAKE_CXX_STANDARD 11)

add_executable(LanguageRef main.c Tree/Tree.h Tree/Tree.c RecursiveDescent/Descent.c RecursiveDescent/Descent.h
        AsmTranslation/ToAsm.c AsmTranslation/ToAsm.h RecursiveDescent/Commands.h)