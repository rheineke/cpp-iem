# Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
# This project is licensed under the terms of the MIT license.

include(GoogleTest)

set(TESTS
    build_test
    config_test
    contract_test
    credentials_test
    event_strategy_test
    http_client_test
    message_test
    order_test
    price_test
    pricetimelimit_test
    session_test
    side_test)

# The tests read conf/markets.json relative to their working directory
configure_file(${PROJECT_SOURCE_DIR}/conf/markets.json conf/markets.json
               COPYONLY)

foreach(test IN LISTS TESTS)
    add_executable(cppiem-iem-${test} ${test}.cpp)
    target_link_libraries(cppiem-iem-${test} PRIVATE cppiemlib cppiem_warnings
                                                     GTest::gtest_main)
    gtest_discover_tests(cppiem-iem-${test}
                         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
