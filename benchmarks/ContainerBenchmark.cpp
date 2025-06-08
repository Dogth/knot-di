#include "../include/Container.hpp"
#include <benchmark/benchmark.h>

static void BM_Container_RegisterManySingletons(benchmark::State &state) {
  struct Dummy {
    int x;
  };

  for (auto _ : state) {
    Knot::Container c;
    for (int i = 0; i < 32; ++i) {
      c.registerService<Dummy>(SINGLETON);
    }
  }
}
BENCHMARK(BM_Container_RegisterManySingletons);

static void BM_Container_ResolveManyTransients(benchmark::State &state) {
  struct Dummy {
    int x;
    Dummy(int v) : x(v) {}
  };

  Knot::Container c;
  for (int i = 0; i < 32; ++i)
    c.registerService<Dummy>(TRANSIENT, i);
  for (auto _ : state) {
    for (int i = 0; i < 8; ++i) {
      Dummy *d = c.resolve<Dummy>();
      benchmark::DoNotOptimize(d);
    }
    c.destroyAllTransients();
  }
}
BENCHMARK(BM_Container_ResolveManyTransients);

static void BM_Container_DestroyAllSingletons(benchmark::State &state) {
  struct Dummy {
    int x;
  };

  Knot::Container c;
  c.registerService<Dummy>(SINGLETON);
  for (auto _ : state) {
    Dummy *d = c.resolve<Dummy>();
    c.destroyAllSingletons();
  }
}
BENCHMARK(BM_Container_DestroyAllSingletons);

static void BM_Container_DestroyAllTransients(benchmark::State &state) {
  struct Dummy {
    int x;
    Dummy(int v) : x(v) {}
  };

  Knot::Container c;
  c.registerService<Dummy>(TRANSIENT, 42);
  for (auto _ : state) {
    Dummy *d = c.resolve<Dummy>();
    benchmark::DoNotOptimize(d);
    c.destroyAllTransients();
  }
}
BENCHMARK(BM_Container_DestroyAllTransients);

static void
BM_Container_RegisterAndResolveSimpleSingleton(benchmark::State &state) {
  struct Dummy {
    int x;
  };

  for (auto _ : state) {
    Knot::Container c;
    c.registerService<Dummy>(SINGLETON);
    Dummy *s = c.resolve<Dummy>();
    benchmark::DoNotOptimize(s);
  }
}
BENCHMARK(BM_Container_RegisterAndResolveSimpleSingleton);

static void
BM_Container_RegisterAndResolveSimpleTransient(benchmark::State &state) {
  struct Dummy {
    int x;
  };

  for (auto _ : state) {
    Knot::Container c;
    c.registerService<Dummy>(TRANSIENT);
    Dummy *s = c.resolve<Dummy>();
    benchmark::DoNotOptimize(s);
  }
}
BENCHMARK(BM_Container_RegisterAndResolveSimpleTransient);

static void BM_Container_ResolveComplex(benchmark::State &state) {
  struct Dep1 {
    int a;
    Dep1(int v) : a(v) {}
  };
  struct Dep2 {
    float b;
    Dep2(float v) : b(v) {}
  };
  struct Complex {
    Dep1 *d1;
    Dep2 *d2;
    Complex(Dep1 *d1_, Dep2 *d2_) : d1(d1_), d2(d2_) {}
  };

  for (auto _ : state) {
    Knot::Container c;
    c.registerService<Dep1>(SINGLETON, 42);
    c.registerService<Dep2>(SINGLETON, 3.14f);
    c.registerService<Complex>(TRANSIENT, c.resolve<Dep1>(), c.resolve<Dep2>());
    Complex *obj = c.resolve<Complex>();
    benchmark::DoNotOptimize(obj);
  }
}
BENCHMARK(BM_Container_ResolveComplex);
