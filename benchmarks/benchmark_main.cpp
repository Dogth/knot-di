#include "../include/Container.hpp"
#include <benchmark/benchmark.h>

#define KNOT_MAX_SERVICES 9999999
#define KNOT_MAX_TRANSIENTS 9999999
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
  c.destroyAllTransients(); // Clean up transients after resolving
  for (auto _ : state) {
    c.resolve<ServiceImpl>();
  }
}
BENCHMARK(BM_ResolveService)->Iterations(4);

BENCHMARK_MAIN();
