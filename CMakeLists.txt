cmake_minimum_required(VERSION 3.26)
project(AccuenergyTest)

set(CMAKE_CXX_STANDARD 11)

add_executable(AccuenergyTest main.cpp)

add_library(server STATIC TCPServer.cpp
        EchoServer.cpp
        simpleMemPool.cpp
)
add_library(client STATIC TCPClient.cpp)

target_link_libraries(AccuenergyTest PRIVATE
        server
        client
)