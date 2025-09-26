cmake_minimum_required(VERSION 3.20)
project(REDIS_SERVER LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/std:c++latest)
endif()

file(GLOB_RECURSE SOURCES "src/*.cc")

add_executable(REDIS_SERVER ${SOURCES})

target_include_directories(REDIS_SERVER PRIVATE src)
