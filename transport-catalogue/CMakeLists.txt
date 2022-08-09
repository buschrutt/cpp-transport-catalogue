cmake_minimum_required(VERSION 3.22)
project(untitled3)

set(CMAKE_CXX_STANDARD 17)

add_executable(testing main.cpp
        testing old_/input_reader.h
        testing old_/stat_reader.h
        testing transport_catalogue.h
        testing old_/input_reader.cpp
        testing old_/stat_reader.cpp
        testing geo.cpp
        testing transport_catalogue.cpp)