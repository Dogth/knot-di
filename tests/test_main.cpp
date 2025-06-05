#include "../include/Container.hpp"
#include <cassert>
#include <gtest/gtest.h>
#include <iostream>

// TODO: refactor
// TODO: add more tests
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

  Container container;
  container.registerService<ServiceImpl>(SINGLETON);

  ServiceImpl *service1 = container.resolve<ServiceImpl>();
  ServiceImpl *service2 = container.resolve<ServiceImpl>();

  ASSERT_NE(service1, nullptr);
  ASSERT_EQ(service1, service2);
  EXPECT_EQ(service1->getValue(), 42);
}

TEST(ContainerTest, RegisterAndResolveTransient) {
  Container container;
  container.registerService<TServiceImpl>(TRANSIENT);

  TServiceImpl *service1 = container.resolve<TServiceImpl>();
  TServiceImpl *service2 = container.resolve<TServiceImpl>();

  ASSERT_NE(service1, nullptr);
  ASSERT_NE(service2, nullptr);
  ASSERT_NE(service1, service2);
  EXPECT_EQ(service1->getValue(), 42);
  EXPECT_EQ(service2->getValue(), 42);

  delete service1;
  delete service2;
}

TEST(ContainerTest, RegisterWithArgAndResolve) {

  char buffer[1024];
  // TODO: generic trick
  Container container(buffer, 1024);
  container.registerService<ServiceImplWithArg>(Strategy::SINGLETON, 99);

  ServiceImplWithArg *service = container.resolve<ServiceImplWithArg>();
  ASSERT_NE(service, nullptr);
  EXPECT_EQ(service->getValue(), 99);
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
