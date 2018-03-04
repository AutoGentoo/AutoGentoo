cmake_minimum_required(VERSION 2.8.9)
project(AutoGentoo-interface)

SET(CMAKE_CXX_COMPILER g++)

add_library (autogentoo-interface SHARED
        VectorWrapper.cpp
        VectorWrapper.h)