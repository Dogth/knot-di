find_package(benchmark REQUIRED)
include_directories(${benchmark_INCLUDE_DIRS})

add_executable(knot-di-benchmarks
		ContainerBenchmark.cpp
		MemoryPoolBenchmark.cpp
		BenchmarkMain.cpp
)

target_link_libraries(knot-di-benchmarks
    knot-di
    benchmark::benchmark
)
