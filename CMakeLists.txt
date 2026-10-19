# Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
# This project is licensed under the terms of the MIT license.

cmake_minimum_required(VERSION 3.2)
project(CPP-IEM)

option(CPP-IEM_BUILD_SHARED_LIBS "Build cpp-iem with shared libraries." OFF)
option(CPP-IEM_BUILD_TESTS "Build the cpp-iem project tests." ON)

include(GNUInstallDirs)

if(CPP-IEM_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    message(STATUS "Linking boost testing libs dynamically...")
    set(CPP-IEM_BUILD_SHARED_LIBS ON)
    set(BUILD_SHARED_LIBS ON)
else()
    set(CPP-IEM_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
endif()

# Find threads (for Linux)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(Threads REQUIRED)
endif()

# Always use Boost's shared libraries.
set(Boost_USE_STATIC_LIBS OFF)

# We need this for all tests to use the dynamic version.
add_definitions(-DBOOST_TEST_DYN_LINK)

# Always use multi-threaded Boost libraries.
set(Boost_USE_MULTI_THREADED ON)

find_package(Boost 1.64.0 REQUIRED COMPONENTS date_time filesystem system
                                              thread)

if(APPLE)
    # If we're on OSX check for Homebrew's copy of OpenSSL instead of Apple's
    if(NOT OpenSSL_DIR)
        find_program(HOMEBREW brew)
        if(HOMEBREW STREQUAL "HOMEBREW-NOTFOUND")
            message(WARNING "Homebrew not found: not using Homebrew's OpenSSL")
            if(NOT OPENSSL_ROOT_DIR)
                message(WARNING "Use -DOPENSSL_ROOT_DIR for non-Apple OpenSSL")
            endif()
        else()
            execute_process(
                COMMAND brew --prefix openssl
                OUTPUT_VARIABLE OPENSSL_ROOT_DIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        endif()
    endif()
endif()
# cpp-httplib (deps/cpp-httplib) needs OpenSSL 3 for HTTPS, and zlib to
# decompress responses
find_package(OpenSSL 3.0 REQUIRED)
find_package(ZLIB REQUIRED)

# set(CMAKE_VERBOSE_MAKEFILE on)

# MUST bump the major version whenever we make ABI-incompatible changes
set(CPP-IEM_VERSION_MAJOR 0)
set(CPP-IEM_VERSION_MINOR 1)
set(CPP-IEM_VERSION_PATCH 1)
set(CPP-IEM_VERSION_STRING
    ${CPP-IEM_VERSION_MAJOR}.${CPP-IEM_VERSION_MINOR}.${CPP-IEM_VERSION_PATCH})

if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
else()
    message(FATAL_ERROR "OpenSSL required for https connection to cpp-iem")
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    # Use C++20 when using GNU compilers.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++20")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    # We want to link in C++20 mode in Clang too, but also set a high enough
    # template depth for the template metaprogramming.
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -ftemplate-depth=256 -std=c++20")
endif()

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

enable_testing()

if(CPP-IEM_BUILD_TESTS)
    add_subdirectory(deps/googletest)
    add_subdirectory(test)
endif()

include_directories("${PROJECT_SOURCE_DIR}/src")
add_subdirectory(src)
# add_subdirectory(src/json)

set(SOURCE_FILES src/main.cpp)
add_executable(CPP-IEM ${SOURCE_FILES})

target_link_libraries(CPP-IEM cppiemlib)

# Config file required for contract_test
file(COPY conf/markets.json DESTINATION ${CPP-IEM_BINARY_DIR}/test/iem/conf)

# ###
# ## Export Targets
# # (so cpp-iem can be easily used by other CMake projects)
# # [see https://cmake.org/cmake/help/latest/guide/importing-exporting/]
