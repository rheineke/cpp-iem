# Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
# This project is licensed under the terms of the MIT license.

cmake_minimum_required(VERSION 3.2)
project(CPP-IEM)

option(CPP-IEM_BUILD_SHARED_LIBS "Build cpp-iem with shared libraries." OFF)
option(CPP-IEM_BUILD_TESTS "Build the cpp-iem project tests." ON)

include(GNUInstallDirs)

if(CPP-IEM_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    message (STATUS "Linking boost testing libs dynamically...")
    set(CPP-IEM_BUILD_SHARED_LIBS ON)
    set(BUILD_SHARED_LIBS ON)
else()
    set(CPP-IEM_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
endif()

# Find threads (for Linux)
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(Threads REQUIRED)
endif()

# Always use Boost's shared libraries.
set(Boost_USE_STATIC_LIBS OFF)

# We need this for all tests to use the dynamic version.
add_definitions(-DBOOST_TEST_DYN_LINK)

# Always use multi-threaded Boost libraries.
set(Boost_USE_MULTI_THREADED ON)

find_package(Boost 1.54.0 REQUIRED COMPONENTS
        date_time
        filesystem
        system
        thread)

if (APPLE)
    # /usr/local/Cellar/cpp-netlib/0.12.0_1/lib/cmake/cppnetlib/cppnetlibTargets-release.cmake
    # adds openssl as part of targets and causes build to fail for some reason
    # make[2]: *** No rule to make target `/usr/local/Cellar/openssl/1.0.2g/lib/libssl.dylib', needed by `CPP-IEM'.  Stop.
    # I have: /usr/local/Cellar/openssl/1.0.2j/...
    # find_package(cppnetlib 0.12.0 REQUIRED)
    set(CPPNETLIB_FOUND ON)
    set(CPPNETLIB_INCLUDE_DIRS /usr/local/Cellar/cpp-netlib/0.12.0_1/include/)
    set(CPPNETLIB_LIBRARIES
            /usr/local/Cellar/cpp-netlib/0.12.0_1/lib/libcppnetlib-client-connections.a
            /usr/local/Cellar/cpp-netlib/0.12.0_1/lib/libcppnetlib-server-parsers.a
            /usr/local/Cellar/cpp-netlib/0.12.0_1/lib/libcppnetlib-uri.a)
elseif (UNIX)
    set(CPPNETLIB_FOUND ON)
    set(CPPNETLIB_LIBRARIES
            /usr/lib/x86_64-linux-gnu/libcppnetlib-uri.so
            # /usr/lib/arm-linux-gnueabihf/libcppnetlib-uri.so
            /usr/lib/x86_64-linux-gnu/libcppnetlib-server-parsers.so
            # /usr/lib/arm-linux-gnueabihf/libcppnetlib-server-parsers.so
            /usr/lib/x86_64-linux-gnu/libcppnetlib-client-connections.so)
            # /usr/lib/arm-linux-gnueabihf/libcppnetlib-client-connections.so)
endif()

if (APPLE)
    # If we're on OSX check for Homebrew's copy of OpenSSL instead of Apple's
    if (NOT OpenSSL_DIR)
        find_program(HOMEBREW brew)
        if (HOMEBREW STREQUAL "HOMEBREW-NOTFOUND")
            message(WARNING "Homebrew not found: not using Homebrew's OpenSSL")
            if (NOT OPENSSL_ROOT_DIR)
                message(WARNING "Use -DOPENSSL_ROOT_DIR for non-Apple OpenSSL")
            endif()
        else()
            execute_process(COMMAND brew --prefix openssl
                    OUTPUT_VARIABLE OPENSSL_ROOT_DIR
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
        endif()
    endif()
endif()
find_package(OpenSSL 1.0.1 REQUIRED)
if (APPLE)
    # Hardcode because first APPLE if statement doesn't work?
    set(OPENSSL_INCLUDE_DIRS /usr/local/opt/openssl/include/)
    set(OPENSSL_LIBRARIES
            /usr/local/opt/openssl/lib/libssl.dylib
            /usr/local/opt/openssl/lib/libcrypto.dylib)
endif()

# set(CMAKE_VERBOSE_MAKEFILE on)

set(CPP-IEM_VERSION_MAJOR 0) # MUST bump this whenever we make ABI-incompatible changes
set(CPP-IEM_VERSION_MINOR 1)
set(CPP-IEM_VERSION_PATCH 1)
set(CPP-IEM_VERSION_STRING
        ${CPP-IEM_VERSION_MAJOR}.${CPP-IEM_VERSION_MINOR}.${CPP-IEM_VERSION_PATCH})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DBOOST_NETWORK_DEBUG)
endif()

if (OPENSSL_FOUND)
    add_definitions(-DBOOST_NETWORK_ENABLE_HTTPS)
    include_directories(${OPENSSL_INCLUDE_DIR})
else()
    message(FATAL_ERROR "OpenSSL required for https connection to cpp-iem")
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU)
    # Use C++1z when using GNU compilers.
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1z")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    # We want to link in C++1z mode in Clang too, but also set a high enough
    # template depth for the template metaprogramming.
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ftemplate-depth=256 -std=c++1z")
endif()

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

if (CPPNETLIB_FOUND)
    include_directories(${CPPNETLIB_INCLUDE_DIRS})

    enable_testing()

    if (CPP-IEM_BUILD_TESTS)
        add_subdirectory(deps/googletest)
        add_subdirectory(test)
    endif()
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
# # [see http://www.cmake.org/Wiki/CMake/Tutorials/How_to_create_a_ProjectConfig.cmake_file]
