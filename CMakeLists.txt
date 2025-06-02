cmake_minimum_required(VERSION 3.10)

project(knot-di VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Knot Dependency Injection Framework")

enable_testing()

add_library(knot-di INTERFACE)

add_subdirectory(tests)
#add_subdirectory(benchmarks)

target_include_directories(knot-di INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS knot-di
    EXPORT knot-di-targets
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)
