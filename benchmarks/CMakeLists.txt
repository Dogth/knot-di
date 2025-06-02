find_package(benchmark REQUIRED)

add_executable(knot-di-benchmarks
    benchmark_main.cpp
    # Add more benchmark source files here
)

target_link_libraries(knot-di-benchmarks
    knot-di
    benchmark::benchmark
)
