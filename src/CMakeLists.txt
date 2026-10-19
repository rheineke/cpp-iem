project(cppiemlib)

# Ordering may matter
include_directories(iem json)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} iem)
add_library(
    cppiemlib
    json/json.h
    json/json-forwards.h
    jsoncpp.cpp
    iem/config.hpp
    iem/config.cpp
    iem/contract.hpp
    iem/contract.cpp
    iem/event_strategy.hpp
    iem/event_strategy.cpp
    iem/fatfinger_strategy.hpp
    iem/fatfinger_strategy.cpp
    iem/http_client.hpp
    iem/http_client.cpp
    iem/logger.hpp
    iem/logger.cpp
    iem/message.hpp
    iem/message.cpp
    iem/order.hpp
    iem/order.cpp
    iem/orderbook.hpp
    iem/orderbook.cpp
    iem/price.hpp
    iem/price.cpp
    iem/pricetimelimit.hpp
    iem/pricetimelimit.cpp
    iem/session.hpp
    iem/session.cpp
    iem/side.hpp
    iem/side.cpp)

# Header-only HTTP client; the definitions must match in every file that
# includes httplib.h, so they are PUBLIC
target_include_directories(cppiemlib SYSTEM
                           PUBLIC ${CPP-IEM_SOURCE_DIR}/deps/cpp-httplib)
target_compile_definitions(cppiemlib PUBLIC CPPHTTPLIB_OPENSSL_SUPPORT
                                            CPPHTTPLIB_ZLIB_SUPPORT)
target_link_libraries(cppiemlib OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB
                      ${Boost_LIBRARIES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(cppiemlib ${CMAKE_THREAD_LIBS_INIT})
endif()
