# include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# set(IEM_SOURCE_FILES contract.hpp contract.cpp order.hpp order.cpp orderbook.hpp orderbook.cpp session.hpp session.cpp)
# add_library(iemlib_iem
        # contract.hpp contract.cpp
        # executedorder.hpp executedorder.cpp
        # order.hpp order.cpp
        # orderbook.hpp orderbook.cpp
        # price.hpp price.cpp
        # session.hpp session.cpp
        # side.hpp side.cpp)

# target_include_directories(iemlib_iem PRIVATE ${Boost_INCLUDE_DIRS})

# target_link_libraries(iemlib json)
# target_include_directories(iemlib PRIVATE ${IEM_SOURCE_FILES})
# target_link_libraries(iemlib ${Boost_LIBRARIES})

# target_include_directories(iem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})