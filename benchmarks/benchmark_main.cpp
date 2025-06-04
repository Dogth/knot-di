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
    // Create a new container for each iteration to avoid duplicate registration
    Container &c = Container::instance();
    c.registerService<ServiceImpl>(Strategy::SINGLETON);
  }
}
BENCHMARK(BM_RegisterService);

static void BM_ResolveService(benchmark::State &state) {
  Container &c = Container::instance();
  c.registerService<ServiceImpl>(Strategy::SINGLETON);
  for (auto _ : state) {
    IService *svc = c.resolve<IService>();
    benchmark::DoNotOptimize(svc);
  }
}
BENCHMARK(BM_ResolveService);

BENCHMARK_MAIN();
