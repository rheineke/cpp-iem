# Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
# This project is licensed under the terms of the MIT license.

add_library(
    cppiemlib
    iem/config.hpp
    iem/config.cpp
    iem/contract.hpp
    iem/contract.cpp
    iem/credentials.hpp
    iem/credentials.cpp
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

target_compile_features(cppiemlib PUBLIC cxx_std_20)
target_include_directories(cppiemlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Header-only HTTP client; the definitions must match in every file that
# includes httplib.h, so they are PUBLIC
target_include_directories(cppiemlib SYSTEM
                           PUBLIC ${PROJECT_SOURCE_DIR}/deps/cpp-httplib)
target_compile_definitions(cppiemlib PUBLIC CPPHTTPLIB_OPENSSL_SUPPORT
                                            CPPHTTPLIB_ZLIB_SUPPORT)
target_link_libraries(
    cppiemlib
    PUBLIC Boost::headers nlohmann_json::nlohmann_json OpenSSL::SSL
           OpenSSL::Crypto ZLIB::ZLIB Threads::Threads
    PRIVATE cppiem_warnings)
# cpp-httplib reads root certificates from the macOS Keychain
if(APPLE)
    target_link_libraries(cppiemlib PUBLIC "-framework CoreFoundation"
                                           "-framework Security")
endif()
