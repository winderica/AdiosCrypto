cmake_minimum_required(VERSION 3.15)
project(Crypto)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgmp -lgmpxx -lssl -lcrypto -lz")

add_executable(Crypto src/main.cpp)