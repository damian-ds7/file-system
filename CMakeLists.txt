cmake_minimum_required(VERSION 3.30)
project(FileSystem)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

file(GLOB SOURCES
        src/*.h
        src/*.cpp
)

add_executable(FileSystem ${SOURCES}
        src/typedefs.h)
