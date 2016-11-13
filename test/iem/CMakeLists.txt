# Copyright 2016 Reece Heineke<reece.heineke@gmail.com>
# This project is licensed under the terms of the MIT license.

include_directories(
        ${OPENSSL_INCLUDE_DIR}
        ${CPP-IEM_SOURCE_DIR}/src
        ${gtest_SOURCE_DIR}/include)

set(TESTS
        config_test
        contract_test
        event_strategy_test
        order_test
        price_test
        pricetimelimit_test
        session_test
        side_test)

foreach ( test ${TESTS} )
    add_executable(cppiem-iem-${test} ${test}.cpp)
    target_link_libraries(cppiem-iem-${test} gtest_main cppiemlib)
    if (${CMAKE_CXX_COMPILER_ID} MATCHES GNU AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        target_link_libraries(cppiem-iem-${test} ws2_32)
    endif()
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(cppiem-iem-${test} rt)
    endif()
    set_target_properties(cppiem-iem-${test}
            PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CPP-IEM_BINARY_DIR}/tests)
    add_test(cppiem-iem-${test}
            ${CPP-IEM_BINARY_DIR}/tests/cppiem-iem-${test}
            WORKING_DIRECTORY ${CPP-IEM_BINARY_DIR})
endforeach (test)