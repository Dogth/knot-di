#ifndef KNOT_UTIL_HPP
#define KNOT_UTIL_HPP

namespace Knot {

// Static storage for singleton instances
template <typename T>
struct SingletonStorage {
  static T* instance;
};
template <typename T>
T* SingletonStorage<T>::instance = 0;

// Static storage for factory functions
template <typename T>
struct FactoryStorage {
  static T* (*factory)();
};
template <typename T>
T* (*FactoryStorage<T>::factory)() = 0;

}  // namespace Knot

#endif  // KNOT_UTIL_HPP
