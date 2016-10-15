include_directories(/usr/local/include/Cellar/boost/1.61.0_1/include)
# include_directories(${Boost_INCLUDE_DIRS})
# include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(iem contract.hpp contract.cpp orderbook.hpp orderbook.cpp)

# target_link_libraries(iemlib json)
target_link_libraries(iem ${Boost_LIBRARIES})

# target_include_directories(iem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})