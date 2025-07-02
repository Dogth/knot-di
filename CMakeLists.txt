cmake_minimum_required(VERSION 3.10)
project(knot-di VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(COVERAGE "Enable coverage reporting" OFF)

add_library(knot-di INTERFACE)
target_include_directories(knot-di INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-Wall" COMPILER_SUPPORTS_WALL)
check_cxx_compiler_flag("-Wextra" COMPILER_SUPPORTS_WEXTRA)
if(COMPILER_SUPPORTS_WALL)
    target_compile_options(knot-di INTERFACE -Wall)
endif()
if(COMPILER_SUPPORTS_WEXTRA)
    target_compile_options(knot-di INTERFACE -Wextra)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR COVERAGE)
    if(COVERAGE)
        message(STATUS "Configuring Coverage build")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    else()
        message(STATUS "Configuring Debug build")
        target_compile_definitions(knot-di INTERFACE DEBUG_BUILD)
        target_compile_options(knot-di INTERFACE -g)
    endif()
    find_package(spdlog REQUIRED)
    target_link_libraries(knot-di INTERFACE spdlog::spdlog)
endif()

install(TARGETS knot-di
    EXPORT knot-di-targets
)
install(EXPORT knot-di-targets
    FILE knot-di-config.cmake
    NAMESPACE knotdi::
    DESTINATION lib/cmake/knot-di
)
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

enable_testing()
add_subdirectory(tests)
add_subdirectory(benchmarks)

if(COVERAGE)
    find_program(LCOV_EXEC lcov)
    find_program(GENHTML_EXEC genhtml)
    if(LCOV_EXEC AND GENHTML_EXEC)
        add_custom_target(coverage
            COMMAND ${CMAKE_MAKE_PROGRAM} test
						COMMAND ${LCOV_EXEC} --capture --directory . --output-file coverage.info --ignore-errors inconsistent,format,unsupported
						COMMAND ${GENHTML_EXEC} ./coverage.info --output-directory coverage-report --ignore-errors category,inconsistent,corrupt,unsupported
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running tests and generating coverage report"
        )
    endif()
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif()

file(GLOB_RECURSE ALL_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp)
find_program(CLANG_FORMAT_EXEC clang-format)
if(CLANG_FORMAT_EXEC)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXEC} -i ${ALL_SOURCE_FILES}
        COMMENT "Formatting source files"
    )
endif()
