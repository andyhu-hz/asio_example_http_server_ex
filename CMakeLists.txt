cmake_minimum_required(VERSION 3.6)
project(asio_example_http_server)

set(CMAKE_CXX_STANDARD 11)

#Enable SSE4.2 in picohttpparser
#add_definitions(-D__SSE4_2__)
#add_compile_options(-march=broadwell)

find_package(Boost 1.50 COMPONENTS system thread filesystem REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

find_package( Threads )

set(SOURCE_FILES
        asio_example_http_server_ex/main.cpp
        asio_example_http_server_ex/connection.cpp
        asio_example_http_server_ex/io_service_pool.cpp
        asio_example_http_server_ex/mime_types.cpp
        asio_example_http_server_ex/reply.cpp
        asio_example_http_server_ex/server.cpp
        asio_example_http_server_ex/picohttpparser.c
        asio_example_http_server_ex/utils.cpp)

add_executable(asio_example_http_server ${SOURCE_FILES})
target_link_libraries(asio_example_http_server ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})