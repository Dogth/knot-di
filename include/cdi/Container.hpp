#ifndef KNOT_CONTAINER_HPP
#define KNOT_CONTAINER_HPP

#include "Factory.hpp"
#include "Util.hpp"

namespace Knot {

struct Container {
  // Register a singleton instance
  template <typename T>
  static void registerInstance(T* inst) {
    SingletonStorage<T>::instance = inst;
  }

  // Register a singleton (constructed via default constructor)
  template <typename T>
  static void registerSingleton() {
    if (!SingletonStorage<T>::instance)
      SingletonStorage<T>::instance = DefaultFactory<T>();
  }

  // Register a factory function
  template <typename T>
  static void registerFactory(T* (*f)()) {
    FactoryStorage<T>::factory = f;
  }

  // Resolve a service (singleton or via factory)
  template <typename T>
  static T* resolve() {
    if (SingletonStorage<T>::instance) return SingletonStorage<T>::instance;
    if (FactoryStorage<T>::factory) return FactoryStorage<T>::factory();
    SingletonStorage<T>::instance = DefaultFactory<T>();
    return SingletonStorage<T>::instance;
  }
};

}  // namespace Knot

#endif  // KNOT_CONTAINER_HPP
