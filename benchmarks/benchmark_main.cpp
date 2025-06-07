#include "../include/Container.hpp"
#include <benchmark/benchmark.h>

using namespace Knot;

// A simple service interface and implementation
struct IService {
  virtual int foo() = 0;
  virtual ~IService() = default;
};

struct ServiceImpl : IService {
  int foo() override { return 42; }
};

static void BM_RegisterService(benchmark::State &state) {
  for (auto _ : state) {
    Container c;
    c.registerService<ServiceImpl>(TRANSIENT);
  }
}
BENCHMARK(BM_RegisterService);

static void BM_ResolveService(benchmark::State &state) {
  Container c;
  c.registerService<ServiceImpl>(TRANSIENT);
  for (auto _ : state) {
    c.resolve<ServiceImpl>();
  }
  c.destroyAllTransients();
  for (auto _ : state) {
    c.resolve<ServiceImpl>();
  }
}
BENCHMARK(BM_ResolveService)->Iterations(16);

BENCHMARK_MAIN();
