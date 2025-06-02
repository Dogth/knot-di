#ifndef DESCRIPTOR_HPP
#define DESCRIPTOR_HPP

#include "IFactory.hpp"
#include "Strategy.hpp"

namespace Knot {
struct Descriptor {
  IFactory *factory;
  Strategy strategy;
  void *instance;

  Descriptor(IFactory *factory, Strategy strategy)
      : factory(factory), strategy(strategy), instance(nullptr) {}

  ~Descriptor() {
    if (strategy == Strategy::SINGLETON && instance != nullptr) {
      factory->deleteInstance(instance);
    }
    delete factory;
  }

private:
  Descriptor(const Descriptor &);
  Descriptor &operator=(const Descriptor &);
};
}; // namespace Knot

#endif // DESCRIPTOR_HPP
