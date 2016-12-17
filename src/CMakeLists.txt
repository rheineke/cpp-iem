project(cppiemlib)

# Ordering may matter
include_directories(iem json)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} iem)
add_library(cppiemlib
        json/json.h json/json-forwards.h
        jsoncpp.cpp
        iem/config.hpp iem/config.cpp
        iem/contract.hpp iem/contract.cpp
        iem/event_strategy.hpp iem/event_strategy.cpp
        iem/fatfinger_strategy.hpp iem/fatfinger_strategy.cpp
        iem/logger.hpp iem/logger.cpp
        iem/message.hpp iem/message.cpp
        iem/order.hpp iem/order.cpp
        iem/orderbook.hpp iem/orderbook.cpp
        iem/price.hpp iem/price.cpp
        iem/pricetimelimit.hpp iem/pricetimelimit.cpp
        iem/session.hpp iem/session.cpp
        iem/side.hpp iem/side.cpp)

target_link_libraries(cppiemlib
        ${OPENSSL_LIBRARIES}
        ${Boost_LIBRARIES}
        ${CPPNETLIB_LIBRARIES})