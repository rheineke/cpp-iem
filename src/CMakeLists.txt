project(iemlib)

# Ordering may matter
include_directories(json)
add_subdirectory(json)

include_directories(iem)
add_subdirectory(iem)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(iemlib jsoncpp.cpp)

target_link_libraries(iemlib iem)