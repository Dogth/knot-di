cmake_minimum_required(VERSION 3.10)

project(knot-di $ENV{KNOT_VERSION} LANGUAGES CXX DESCRIPTION "Knot Dependency Injection Framework")

enable_testing()

add_library(knot-di STATIC )
	#TODO: add pahs to source files
)

add_subdirectory(tests)
add_subdirectory(benchmarks)

target_include_directories(knot-di PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

install (TARGETS knot-di
		EXPORT knot-di-targets
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
)

install (DiRECTORY include/)
		DESTINATION include
		FILES_MATCHING PATTERN "*.hpp"
)




