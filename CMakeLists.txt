cmake_minimum_required(VERSION 3.2)
project(asio_example_http_server)

set(CMAKE_CXX_STANDARD 11)

#Enable SSE4.2 in picohttpparser
#add_definitions(-D__SSE4_2__)
#add_compile_options(-march=broadwell)

find_package(Boost 1.55 COMPONENTS system thread filesystem locale REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package( Threads )

set(SOURCE_FILES
        asio_example_http_server_ex/main.cpp
        asio_example_http_server_ex/io_service_pool.cpp
        asio_example_http_server_ex/mime_types.cpp
        asio_example_http_server_ex/reply.cpp
        asio_example_http_server_ex/server.cpp
        asio_example_http_server_ex/picohttpparser.c
        asio_example_http_server_ex/utils.cpp
        asio_example_http_server_ex/request.cpp
        asio_example_http_server_ex/multipart_parser.c
        asio_example_http_server_ex/websocket.cpp)

add_executable(asio_example_http_server ${SOURCE_FILES})
target_link_libraries(asio_example_http_server
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${OPENSSL_LIBRARIES})