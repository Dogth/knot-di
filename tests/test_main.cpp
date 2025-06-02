#include "../include/Container.hpp"
#include <gtest/gtest.h>

using namespace Knot;

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
  auto &container = Container::instance();
  container.registerService<IService, ServiceImpl>(Strategy::SINGLETON);

  IService *service1 = container.resolve<IService>();
  IService *service2 = container.resolve<IService>();

  ASSERT_NE(service1, nullptr);
  ASSERT_EQ(service1, service2);
  EXPECT_EQ(service1->getValue(), 42);
}

TEST(ContainerTest, RegisterAndResolveTransient) {
  auto &container = Container::instance();
  container.registerService<TService, TServiceImpl>(Strategy::TRANSIENT);

  TService *service1 = container.resolve<TService>();
  TService *service2 = container.resolve<TService>();

  ASSERT_NE(service1, nullptr);
  ASSERT_NE(service2, nullptr);
  ASSERT_NE(service1, service2);
  EXPECT_EQ(service1->getValue(), 42);
  EXPECT_EQ(service2->getValue(), 42);

  delete service1;
  delete service2;
}

TEST(ContainerTest, RegisterWithArgAndResolve) {
  auto &container = Container::instance();
  container.registerService<ServiceImplWithArg, int>(Strategy::SINGLETON, 99);

  ServiceImplWithArg *service = container.resolve<ServiceImplWithArg>();
  ASSERT_NE(service, nullptr);
  EXPECT_EQ(service->getValue(), 99);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
