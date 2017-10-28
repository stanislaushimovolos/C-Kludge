cmake_minimum_required(VERSION 3.8)
project(VCPU)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=DEF")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D=RELEASE")
set(SOURCE_FILES main.cpp CPU.h)
add_executable(VCPU ${SOURCE_FILES})