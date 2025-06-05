find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${spdlog_INCLUDE_DIRS})

add_executable(knot-di-tests
    test_main.cpp
)
 
target_link_libraries(knot-di-tests
    knot-di
    GTest::GTest
    GTest::Main
		spdlog::spdlog
)

add_test(NAME knot-di-tests COMMAND knot-di-tests)
