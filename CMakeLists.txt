cmake_minimum_required(VERSION 3.0)

project(mbtest)

add_executable(mbtest main.cpp)

install(TARGETS mbtest RUNTIME DESTINATION bin)
