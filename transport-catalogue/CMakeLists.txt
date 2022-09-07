cmake_minimum_required(VERSION 3.22)
project(untitled3)

set(CMAKE_CXX_STANDARD 17)

add_executable(testing main.cpp
        transport_catalogue.h
        geo.cpp
        domain.cpp
        json.cpp
        json_reader.cpp
        map_renderer.cpp
        request_handler.cpp
        json_builder.cpp
        svg.cpp
        request_handler.cpp
        transport_catalogue.cpp)