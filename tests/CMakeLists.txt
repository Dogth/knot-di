find_package(gtest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(knot-di-tests
    test_main.cpp
    # Add more test source files here
)

target_link_libraries(knot-di-tests
    knot-di
    GTest::GTest
    GTest::Main
)

add_test(NAME knot-di-tests COMMAND knot-di-tests)
