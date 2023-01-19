cmake_minimum_required(VERSION 3.22)
project(TransportCatalogue)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH protobuf-cxxpac-x86-64)
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_catalogue.proto map_renderer.proto svg.proto transport_router.proto)

set(CATALOGUE_FILES main.cpp transport_catalogue.h geo.cpp domain.cpp json.cpp json_reader.cpp map_renderer.cpp
        request_handler.cpp json_builder.cpp svg.cpp request_handler.cpp transport_catalogue.cpp transport_router.cpp
        serialization.cpp serialization.h)

add_executable(transport_catalogue ${PROTO_SRCS} ${PROTO_HDRS} ${CATALOGUE_FILES})

target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(transport_catalogue "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)
