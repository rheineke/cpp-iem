# Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
# This project is licensed under the terms of the MIT license.

cmake_minimum_required(VERSION 3.6)
project(IEM)

option( IEM_BUILD_SHARED_LIBS "Build IEM with shared libraries." OFF )
option( IEM_BUILD_TESTS "Build the IEM project tests." ON)
option( IEM_ENABLE_HTTPS "Build IEM with support for https if OpenSSL is found." ON)

include(GNUInstallDirs)

if(IEM_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    message (STATUS "Linking boost testing libs dynamically...")
    set(IEM_BUILD_SHARED_LIBS ON)
    set(BUILD_SHARED_LIBS ON)
else()
    set(IEM_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
endif()

# Always use Boost's shared libraries.
set(Boost_USE_STATIC_LIBS OFF)

# We need this for all tests to use the dynamic version.
add_definitions(-DBOOST_TEST_DYN_LINK)

# Always use multi-threaded Boost libraries.
set(Boost_USE_MULTI_THREADED ON)

find_package(Boost 1.62.0 REQUIRED COMPONENTS date_time system)

if (IEM_ENABLE_HTTPS)
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
                message(WARNING ${OPENSSL_ROOT_DIR})
                execute_process(COMMAND brew --prefix openssl
                        OUTPUT_VARIABLE OPENSSL_ROOT_DIR
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
            endif()
        endif()
    endif()
    message(WARNING ${OPENSSL_ROOT_DIR})
    find_package(OpenSSL 1.0.2 REQUIRED)
    message(WARNING ${OPENSSL_ROOT_DIR})
endif()

set(CMAKE_VERBOSE_MAKEFILE on)

set(IEM_VERSION_MAJOR 0) # MUST bump this whenever we make ABI-incompatible changes
set(IEM_VERSION_MINOR 1)
set(IEM_VERSION_PATCH 0)
set(IEM_VERSION_STRING ${IEM_VERSION_MAJOR}.${IEM_VERSION_MINOR}.${IEM_VERSION_PATCH})

if (OPENSSL_FOUND)
    message(WARNING ${OPEN_SSL_INCLUDE_DIR})
    add_definitions(-DBOOST_NETWORK_ENABLE_HTTPS)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

# find_package(OpenSSL 1.0.2 REQUIRED)
set(CPPNETLIB_INCLUDE_DIRS /usr/local/Cellar/cpp-netlib/0.12.0_1/include/)
set(CPPNETLIB_LIBRARIES
        /usr/local/Cellar/cpp-netlib/0.12.0_1/lib/libcppnetlib-client-connections.a
        /usr/local/Cellar/cpp-netlib/0.12.0_1/lib/libcppnetlib-server-parsers.a
        /usr/local/Cellar/cpp-netlib/0.12.0_1/lib/libcppnetlib-uri.a)

# if(OPENSSL_FOUND)
    # Nuclear option
#     set(OPENSSL_INCLUDE_DIRS /usr/local/opt/openssl/include/)
MESSAGE(WARNING "OpenSSL include: ${OPENSSL_INCLUDE_DIRS}")
#     set(OPENSSL_LIBRARIES /usr/local/opt/openssl/lib/libssl.dylib;/usr/local/opt/openssl/lib/libcrypto.dylib)
MESSAGE(WARNING "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
# endif()

if(Boost_FOUND)
    MESSAGE(WARNING "Boost includes: ${Boost_INCLUDE_DIRS}")
    MESSAGE(WARNING "Boost dir: ${Boost_DIR}")
    MESSAGE(WARNING "Boost link dirs: ${Boost_LINK_DIRS}")
    MESSAGE(WARNING "Boost libraries: ${Boost_LIBRARIES}")
endif()

MESSAGE(WARNING "cpp-netlib includes: ${CPPNETLIB_INCLUDE_DIRS}")
MESSAGE(WARNING "cpp-netlib link dirs: ${CPPNETLIB_LINK_DIRS}")
MESSAGE(WARNING "cpp-netlib libraries: ${CPPNETLIB_LIBRARIES}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

include_directories("${PROJECT_SOURCE_DIR}/src")
add_subdirectory(src)

set(SOURCE_FILES src/main.cpp)
add_executable(IEM ${SOURCE_FILES})

target_link_libraries(IEM iemlib)

target_include_directories(IEM PUBLIC ${OPENSSL_INCLUDE_DIRS})
target_link_libraries(IEM ${OPENSSL_LIBRARIES})

target_include_directories(IEM PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(IEM ${Boost_LIBRARIES})

target_include_directories(IEM PUBLIC ${CPPNETLIB_INCLUDE_DIRS})
target_link_libraries(IEM ${CPPNETLIB_LIBRARIES})