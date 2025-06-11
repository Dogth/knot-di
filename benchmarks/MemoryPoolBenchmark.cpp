#include <benchmark/benchmark.h>

#include "../include/knot-di/MemoryPool.hpp"

static void BM_MemoryPool_AllocateDeallocate(benchmark::State& state) {
  Knot::MemoryPool pool(256);
  for (auto _ : state) {
    void* ptr = pool.allocate(64, alignof(int));
    benchmark::DoNotOptimize(ptr);
    pool.deallocate(ptr, 64);
  }
}
BENCHMARK(BM_MemoryPool_AllocateDeallocate);

static void BM_MemoryPool_BufferOverflow(benchmark::State& state) {
  char buffer[64];
  Knot::MemoryPool pool(buffer, sizeof(buffer));
  for (auto _ : state) {
    void* ptr1 = pool.allocate(60, alignof(int));
    benchmark::DoNotOptimize(ptr1);
    void* ptr2 = pool.allocate(8, alignof(int));
    benchmark::DoNotOptimize(ptr2);
    pool.reset();
  }
}
BENCHMARK(BM_MemoryPool_BufferOverflow);

static void BM_MemoryPool_Alignment(benchmark::State& state) {
  char buffer[128];
  Knot::MemoryPool pool(buffer, sizeof(buffer));
  for (auto _ : state) {
    void* ptr = pool.allocate(32, 32);
    benchmark::DoNotOptimize(ptr);
    pool.reset();
  }
}
BENCHMARK(BM_MemoryPool_Alignment);

static void BM_MemoryPool_Reset(benchmark::State& state) {
  char buffer[128];
  Knot::MemoryPool pool(buffer, sizeof(buffer));
  for (auto _ : state) {
    void* ptr = pool.allocate(64, alignof(int));
    benchmark::DoNotOptimize(ptr);
    pool.reset();
  }
}
BENCHMARK(BM_MemoryPool_Reset);
