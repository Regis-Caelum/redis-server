cmake_minimum_required(VERSION 3.20)
project(REDIS_SERVER LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/std:c++latest)
endif()

# -----------------------
# Sources
# -----------------------
file(GLOB_RECURSE REDIS_SOURCES "src/*.cc")
list(REMOVE_ITEM REDIS_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cc")
include_directories(src)

# -----------------------
# Build REDIS as a library
# -----------------------
add_library(REDIS_LIB ${REDIS_SOURCES})

# -----------------------
# Server executable
# -----------------------
add_executable(REDIS_SERVER src/main.cc)
target_link_libraries(REDIS_SERVER REDIS_LIB)

# -----------------------
# Optionally build tests
# -----------------------
option(BUILD_TESTS "Build GoogleTest-based tests" OFF)

if(BUILD_TESTS)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    )

    # Only build GTest for this project
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    file(GLOB_RECURSE TEST_SOURCES "tests/*.cc")
    add_executable(REDIS_TESTS ${TEST_SOURCES})
    target_link_libraries(REDIS_TESTS REDIS_LIB gtest_main)

    add_test(NAME REDIS_TESTS COMMAND REDIS_TESTS)
endif()
