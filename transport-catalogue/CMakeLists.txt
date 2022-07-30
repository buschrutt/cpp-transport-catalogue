cmake_minimum_required(VERSION 3.22)
project(untitled3)

set(CMAKE_CXX_STANDARD 17)

add_executable(testing main.cpp
        testing input_reader.h
        testing stat_reader.h
        testing transport_catalogue.h
        testing input_reader.cpp
        testing stat_reader.cpp
        testing transport_catalogue.cpp)