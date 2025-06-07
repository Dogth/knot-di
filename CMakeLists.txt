cmake_minimum_required(VERSION 3.10)
project(knot-di VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Knot Dependency Injection library for C++ adhering to C++03 standard")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

enable_testing()

add_library(knot-di INTERFACE)

add_subdirectory(tests)
add_subdirectory(benchmarks)

target_include_directories(knot-di INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_compile_options(-Wall -Wextra)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
find_package(spdlog REQUIRED)
	target_link_libraries(knot-di INTERFACE spdlog::spdlog)
    message(STATUS "Configuring Debug build")
    add_compile_definitions(DEBUG_BUILD)
		add_compile_options(-fsanitize=address)
    add_compile_options(-g)
endif()

install(TARGETS knot-di
    EXPORT knot-di-targets
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

## Dead code elumination(kinda shoulda use it with DI due to X-macros codegen)
##
## target_link_libraries(firmware PRIVATE knot-di)
##
## target_compile_options(firmware PRIVATE
##     $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-ffunction-sections>
##     $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fdata-sections>
##     $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:-ffunction-sections>
##     $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:-fdata-sections>
## )
##
## target_link_options(firmware PRIVATE
##     $<$<OR:$<LINK_LANG_AND_ID:C,GNU>,$<LINK_LANG_AND_ID:CXX,GNU>>:-Wl,--gc-sections>
## )
