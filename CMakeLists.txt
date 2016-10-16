cmake_minimum_required(VERSION 3.6)
project(iem)

set( CMAKE_VERBOSE_MAKEFILE on )

set(OPENSSL_ROOT_DIR /usr/local/opt/openssl/)

find_package(Boost 1.62.0 REQUIRED)
find_package(OpenSSL 1.0.2 REQUIRED)
set(CPPNETLIB_INCLUDE_DIRS /usr/local/Cellar/cpp-netlib/0.12.0_1/include/)
set(CPPNETLIB_LIBRARIES /usr/local/Cellar/cpp-netlib/0.12.0_1/lib/)

if(OPENSSL_FOUND)
    # Nuclear option
    set(OPENSSL_INCLUDE_DIRS /usr/local/opt/openssl/include/)
    MESSAGE(WARNING "OpenSSL: ${OPENSSL_INCLUDE_DIRS}")
endif()

if(Boost_FOUND)
    MESSAGE(WARNING "${Boost_INCLUDE_DIRS}")
endif()

MESSAGE(WARNING "${CPPNETLIB_INCLUDE_DIRS}")
MESSAGE(WARNING "${CPPNETLIB_LIBRARIES}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")

include_directories("${PROJECT_SOURCE_DIR}/src")
add_subdirectory(src)

set(SOURCE_FILES src/main.cpp)
add_executable(iem ${SOURCE_FILES})

target_link_libraries(iem iemlib)

target_include_directories(iem INTERFACE ${OPENSSL_INCLUDE_DIRS})
target_link_libraries(iem ${OPENSSL_LIBRARIES})

target_include_directories(iem INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(iem ${Boost_LIBRARIES})

target_include_directories(iem PRIVATE ${CPPNETLIB_INCLUDE_DIRS})
target_link_libraries(iem ${CPPNETLIB_LIBRARIES})