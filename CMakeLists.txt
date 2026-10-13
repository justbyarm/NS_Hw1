cmake_minimum_required(VERSION 4.0)
project(untitled25)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled25 main.cpp
        ShrdPtr.h
        UnqPtr.h
        MsPtr.h
        MemorySpan.h)
