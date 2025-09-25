cmake_minimum_required(VERSION 3.20)
project(REDIS_SERVER LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Automatically collect all .cpp files in src/ and subdirectories
file(GLOB_RECURSE SOURCES "src/*.cc")

add_executable(REDIS_SERVER ${SOURCES})

# Include all src/ directories so headers can be found
target_include_directories(REDIS_SERVER PRIVATE src)
