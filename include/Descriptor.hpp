#ifndef DESCRIPTOR_HPP
#define DESCRIPTOR_HPP

#include "Factory.hpp"
#include "Strategy.hpp"

namespace Knot {
struct Descriptor {
  IFactory *factory;
  Strategy strategy;
  void *instance;
  void *storage;

  Descriptor() : factory(0), strategy(SINGLETON), instance(0), storage(0) {}

private:
  Descriptor(const Descriptor &);
  Descriptor &operator=(const Descriptor &);
};
}; // namespace Knot

#endif // DESCRIPTOR_HPP
