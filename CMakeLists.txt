cmake_minimum_required(VERSION 3.6)
project(iem)

set( CMAKE_VERBOSE_MAKEFILE on )

set(OPENSSL_ROOT_DIR /usr/local/opt/openssl/)

find_package(Boost 1.62.0 REQUIRED)
find_package(OpenSSL 1.0.2 REQUIRED)
set(CPPNETLIB_INCLUDE_DIRS /usr/local/Cellar/cpp-netlib/0.12.0_1/include/)
set(CPPNETLIB_LIBRARIES
        /usr/local/Cellar/cpp-netlib/0.12.0_1/lib/libcppnetlib-client-connections.a
        /usr/local/Cellar/cpp-netlib/0.12.0_1/lib/libcppnetlib-server-parsers.a
        /usr/local/Cellar/cpp-netlib/0.12.0_1/lib/libcppnetlib-uri.a)

if(OPENSSL_FOUND)
    # Nuclear option
    set(OPENSSL_INCLUDE_DIRS /usr/local/opt/openssl/include/)
    MESSAGE(WARNING "OpenSSL include: ${OPENSSL_INCLUDE_DIRS}")
    set(OPENSSL_LIBRARIES /usr/local/opt/openssl/lib/libssl.dylib;/usr/local/opt/openssl/lib/libcrypto.dylib)
    MESSAGE(WARNING "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
endif()

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
add_executable(iem ${SOURCE_FILES})

target_link_libraries(iem iemlib)

target_include_directories(iem PUBLIC ${OPENSSL_INCLUDE_DIRS})
target_link_libraries(iem ${OPENSSL_LIBRARIES})

target_include_directories(iem PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(iem ${Boost_LIBRARIES})

target_include_directories(iem PUBLIC ${CPPNETLIB_INCLUDE_DIRS})
target_link_libraries(iem ${CPPNETLIB_LIBRARIES})