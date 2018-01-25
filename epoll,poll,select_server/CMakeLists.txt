cmake_minimum_required(VERSION 2.8)

project(server_client CXX)
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)

add_executable(epoll_server		epoll_server.cc)

add_executable(poll_server	poll_server.cc)

add_executable(select_server	select_server.cc)

add_executable(socket_sin	socket_sin.cc)