# Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
# This project is licensed under the terms of the MIT license.

cmake_minimum_required(VERSION 3.25)
project(
    cpp-iem
    VERSION 0.1.1
    LANGUAGES CXX)

option(CPPIEM_BUILD_TESTS "Build the cpp-iem tests" ${PROJECT_IS_TOP_LEVEL})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
# Header-only Boost libraries: date_time, property_tree, algorithm, pool
find_package(Boost 1.83 REQUIRED CONFIG)
# Homebrew's OpenSSL is keg-only, so CMake does not find it on its own
if(APPLE AND NOT OPENSSL_ROOT_DIR)
    execute_process(
        COMMAND brew --prefix openssl@3
        OUTPUT_VARIABLE OPENSSL_ROOT_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
endif()
# cpp-httplib needs OpenSSL 3 for HTTPS, and zlib to decompress responses
find_package(OpenSSL 3.0 REQUIRED)
find_package(ZLIB REQUIRED)

# Compiler warnings for cpp-iem's own targets; vendored code keeps its own
add_library(cppiem_warnings INTERFACE)
target_compile_options(cppiem_warnings
                       INTERFACE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall>)

add_subdirectory(deps/json SYSTEM)
add_subdirectory(src)

add_executable(CPP-IEM src/main.cpp)
target_link_libraries(CPP-IEM PRIVATE cppiemlib cppiem_warnings)

if(CPPIEM_BUILD_TESTS)
    enable_testing()
    set(BUILD_GMOCK OFF)
    set(INSTALL_GTEST OFF)
    add_subdirectory(deps/googletest SYSTEM)
    add_subdirectory(test)
endif()
