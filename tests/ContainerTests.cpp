#include "../include/Container.hpp"
#include <cassert>
#include <gtest/gtest.h>

TEST(ContainerTest, RegisterAndResolveSingleton) {
  struct Dummy {
    int x = 42;
  };

  Knot::Container container;
  container.registerService<Dummy>(SINGLETON);

  Dummy *service1 = container.resolve<Dummy>();
  Dummy *service2 = container.resolve<Dummy>();

  ASSERT_NE(service1, nullptr);
  ASSERT_EQ(service1, service2);
  EXPECT_EQ(service1->x, 42);
}

TEST(ContainerTest, RegisterAndResolveTransient) {
  struct Dummy {
    int x = 42;
  };

  Knot::Container container;
  container.registerService<Dummy>(TRANSIENT);

  Dummy *service1 = container.resolve<Dummy>();
  Dummy *service2 = container.resolve<Dummy>();

  ASSERT_NE(service1, nullptr);
  ASSERT_NE(service2, nullptr);
  ASSERT_NE(service1, service2);
  EXPECT_EQ(service1->x, 42);
  EXPECT_EQ(service2->x, 42);
}

TEST(ContainerTest, RegisterWithArgAndResolve) {
  struct Dummy {
    int x = 42;
    Dummy(int arg) : x(arg) {}
  };

  Knot::Container container;
  container.registerService<Dummy>(SINGLETON, 99);

  Dummy *service = container.resolve<Dummy>();
  ASSERT_NE(service, nullptr);
  EXPECT_EQ(service->x, 99);
}

TEST(ContainerTest, ResolveUnregisteredService) {
  struct Dummy {};

  Knot::Container container;
  Dummy *dummy = container.resolve<Dummy>();
  ASSERT_EQ(dummy, nullptr);
}

TEST(ContainerTest, RegisterMultipleServices) {
  struct DummyS {
    int x = 42;
  };
  struct DummyT {
    int x = 84;
  };

  Knot::Container container;
  container.registerService<DummyS>(SINGLETON);
  container.registerService<DummyT>(TRANSIENT);

  DummyS *s = container.resolve<DummyS>();
  DummyT *t = container.resolve<DummyT>();

  ASSERT_NE(s, nullptr);
  ASSERT_NE(t, nullptr);
  EXPECT_EQ(s->x, 42);
  EXPECT_EQ(t->x, 84);
}

TEST(ContainerTest, DestroyAllSingletons) {
  struct Dummy {
    int x = 42;
  };

  Knot::Container container;
  container.registerService<Dummy>(SINGLETON);
  Dummy *s = container.resolve<Dummy>();
  ASSERT_NE(s, nullptr);
  container.destroyAllSingletons();

  Dummy *s2 = container.resolve<Dummy>();
  ASSERT_NE(s2, nullptr);
  EXPECT_EQ(s2->x, 42);
}

TEST(ContainerTest, DestroyAllTransient) {
  struct Dummy {
    int x = 42;
  };

  Knot::Container container;
  container.registerService<Dummy>(TRANSIENT);
  Dummy *t1 = container.resolve<Dummy>();
  Dummy *t3 = container.resolve<Dummy>();
  ASSERT_NE(t1, nullptr);
  container.destroyAllTransients();
  Dummy *t2 = container.resolve<Dummy>();
  ASSERT_NE(t2, nullptr);
  EXPECT_EQ(t2->x, 42);
  EXPECT_NE(t1, t2);
}

TEST(ContainerTest, DependencyInjectionWorks) {
  struct Dep {
    int x = 42;
  };
  struct Consumer {
    Dep *dep;
    Consumer(Dep *d) : dep(d) {}
  };

  Knot::Container container;
  container.registerService<Dep>(SINGLETON);
  container.registerService<Consumer>(TRANSIENT, container.resolve<Dep>());

  Consumer *consumer1 = container.resolve<Consumer>();
  Consumer *consumer2 = container.resolve<Consumer>();
  ASSERT_NE(consumer1, nullptr);
  ASSERT_NE(consumer2, nullptr);
  ASSERT_NE(consumer1, consumer2);
  EXPECT_EQ(consumer1->dep, consumer2->dep);
  EXPECT_EQ(consumer1->dep->x, 42);
  EXPECT_EQ(consumer2->dep->x, 42);
  Dep *dep = container.resolve<Dep>();
  ASSERT_EQ(consumer1->dep, dep);
  ASSERT_EQ(consumer2->dep, dep);
}

TEST(ContainerTest, ContainerDoesNotAffectNearbyMemory) {
  alignas(16) char before[32];
  alignas(16) char buffer[256];
  alignas(16) char after[32];

  std::fill_n(before, sizeof(before), 0xAB);
  std::fill_n(after, sizeof(after), 0xCD);

  struct Dummy {
    int x = 123;
  };

  struct Transient {
    int x = 456;
  };

  Knot::Container container(buffer, sizeof(buffer));
  container.registerService<Dummy>(SINGLETON);
  Dummy *dummy = container.resolve<Dummy>();
  ASSERT_NE(dummy, nullptr);
  EXPECT_EQ(dummy->x, 123);

  container.registerService<Transient>(TRANSIENT);
  Transient *t1 = container.resolve<Transient>();
  ASSERT_NE(t1, nullptr);
  EXPECT_EQ(t1->x, 456);

  container.destroyAllTransients();
  container.destroyAllSingletons();

  for (size_t i = 0; i < sizeof(before); ++i) {
    EXPECT_EQ(before[i], static_cast<char>(0xAB))
        << "Container buffer underflow at index " << i;
  }
  for (size_t i = 0; i < sizeof(after); ++i) {
    EXPECT_EQ(after[i], static_cast<char>(0xCD))
        << "Container buffer overflow at index " << i;
  }
}

struct DummySingleton {
  static int destructed;
  DummySingleton() = default;
  ~DummySingleton() { ++destructed; }
};

struct DummyTransient {
  static int destructed;
  DummyTransient() = default;
  ~DummyTransient() { ++destructed; }
};

int DummySingleton::destructed = 0;
int DummyTransient::destructed = 0;

TEST(ContainerTest, DestructorDeletesAllSingletonsAndTransients) {

  int initialSingleton = DummySingleton::destructed = 0;
  int initialTransient = DummyTransient::destructed = 0;

  {
    Knot::Container container;
    container.registerService<DummySingleton>(SINGLETON);
    container.registerService<DummyTransient>(TRANSIENT);
    DummySingleton *s = container.resolve<DummySingleton>();
    (void)s;
    DummyTransient *t1 = container.resolve<DummyTransient>();
    DummyTransient *t2 = container.resolve<DummyTransient>();
    (void)t1;
    (void)t2;
  }

  EXPECT_GT(DummySingleton::destructed, initialSingleton);
  EXPECT_GT(DummyTransient::destructed, initialTransient);
}
