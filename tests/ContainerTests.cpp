#include "../include/Container.hpp"
#include <cassert>
#include <gtest/gtest.h>

struct IService {
  virtual int getValue() const = 0;
  virtual ~IService() = default;
};

struct TService {
  virtual int getValue() const = 0;
  virtual ~TService() = default;
};

struct TServiceImpl : TService {
  int getValue() const override { return 42; }
};

struct ServiceImpl : IService {
  int getValue() const override { return 42; }
};

struct ServiceImplWithArg : IService {
  int value;
  ServiceImplWithArg(int v) : value(v) {}
  int getValue() const override { return value; }
};

TEST(ContainerTest, RegisterAndResolveSingleton) {

  Knot::Container container;
  container.registerService<ServiceImpl>(SINGLETON);

  ServiceImpl *service1 = container.resolve<ServiceImpl>();
  ServiceImpl *service2 = container.resolve<ServiceImpl>();

  ASSERT_NE(service1, nullptr);
  ASSERT_EQ(service1, service2);
  EXPECT_EQ(service1->getValue(), 42);
}

TEST(ContainerTest, RegisterAndResolveTransient) {
  Knot::Container container;
  container.registerService<TServiceImpl>(TRANSIENT);

  TServiceImpl *service1 = container.resolve<TServiceImpl>();
  TServiceImpl *service2 = container.resolve<TServiceImpl>();

  ASSERT_NE(service1, nullptr);
  ASSERT_NE(service2, nullptr);
  ASSERT_NE(service1, service2);
  EXPECT_EQ(service1->getValue(), 42);
  EXPECT_EQ(service2->getValue(), 42);
}

TEST(ContainerTest, RegisterWithArgAndResolve) {
  Knot::Container container;
  container.registerService<ServiceImplWithArg>(Strategy::SINGLETON, 99);

  ServiceImplWithArg *service = container.resolve<ServiceImplWithArg>();
  ASSERT_NE(service, nullptr);
  EXPECT_EQ(service->getValue(), 99);
}

TEST(ContainerTest, ResolveUnregisteredService) {
  Knot::Container container;
  struct Dummy {};
  Dummy *dummy = container.resolve<Dummy>();
  ASSERT_EQ(dummy, nullptr);
}
TEST(ContainerTest, RegisterMultipleServices) {
  Knot::Container container;
  container.registerService<ServiceImpl>(SINGLETON);
  container.registerService<TServiceImpl>(TRANSIENT);

  ServiceImpl *s = container.resolve<ServiceImpl>();
  TServiceImpl *t = container.resolve<TServiceImpl>();

  ASSERT_NE(s, nullptr);
  ASSERT_NE(t, nullptr);
  EXPECT_EQ(s->getValue(), 42);
  EXPECT_EQ(t->getValue(), 42);
}

TEST(ContainerTest, DestroyAllSingletons) {
  Knot::Container container;
  container.registerService<ServiceImpl>(SINGLETON);
  ServiceImpl *s = container.resolve<ServiceImpl>();
  ASSERT_NE(s, nullptr);
  container.destroyAllSingletons();

  ServiceImpl *s2 = container.resolve<ServiceImpl>();
  ASSERT_NE(s2, nullptr);
  EXPECT_EQ(s2->getValue(), 42);
}

TEST(ContainerTest, DestroyAllTransient) {
  Knot::Container container;
  container.registerService<ServiceImpl>(TRANSIENT);
  ServiceImpl *t1 = container.resolve<ServiceImpl>();
  ServiceImpl *t3 = container.resolve<ServiceImpl>();
  ASSERT_NE(t1, nullptr);
  container.destroyAllTransients();
  ServiceImpl *t2 = container.resolve<ServiceImpl>();
  ASSERT_NE(t2, nullptr);
  EXPECT_EQ(t2->getValue(), 42);
  EXPECT_NE(t1, t2);
}

TEST(ContainerTest, DependencyInjectionWorks) {
  struct Dep : IService {
    int getValue() const override { return 7; }
  };
  struct Consumer : IService {
    Dep *dep;
    Consumer(Dep *d) : dep(d) {}
    int getValue() const override { return dep ? dep->getValue() + 1 : -1; }
  };

  Knot::Container container;
  container.registerService<Dep>(SINGLETON);
  container.registerService<Consumer>(TRANSIENT, container.resolve<Dep>());

  Consumer *consumer1 = container.resolve<Consumer>();
  Consumer *consumer2 = container.resolve<Consumer>();
  ASSERT_NE(consumer1, nullptr);
  ASSERT_NE(consumer2, nullptr);
  ASSERT_NE(consumer1, consumer2);
  EXPECT_EQ(consumer1->getValue(), 8);
  EXPECT_EQ(consumer2->getValue(), 8);
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

  Knot::Container container(buffer, sizeof(buffer));

  struct Dummy : IService {
    int getValue() const override { return 123; }
  };
  container.registerService<Dummy>(SINGLETON);
  Dummy *dummy = container.resolve<Dummy>();
  ASSERT_NE(dummy, nullptr);
  EXPECT_EQ(dummy->getValue(), 123);

  struct Transient : IService {
    int getValue() const override { return 456; }
  };
  container.registerService<Transient>(TRANSIENT);
  Transient *t1 = container.resolve<Transient>();
  ASSERT_NE(t1, nullptr);
  EXPECT_EQ(t1->getValue(), 456);

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
