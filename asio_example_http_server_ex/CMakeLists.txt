cmake_minimum_required(VERSION 3.6)
project(asio_example_http_server)

set(CMAKE_CXX_STANDARD 11)

#Enable SSE4.2 in picohttpparser
#add_definitions(-D__SSE4_2__)
#add_compile_options(-march=broadwell)

find_package(Boost 1.50 COMPONENTS system thread REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

find_package( Threads )

set(SOURCE_FILES
        main.cpp
        connection.cpp
        io_service_pool.cpp
        mime_types.cpp
        reply.cpp
        request_handler.cpp
        server.cpp
        picohttpparser.c)

add_executable(asio_example_http_server ${SOURCE_FILES})
target_link_libraries(asio_example_http_server ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})