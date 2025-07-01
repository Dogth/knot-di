#ifndef KNOT_FACTORY_HPP
#define KNOT_FACTORY_HPP

namespace Knot {

// Default factory for type T (no-arg constructor)
template <typename T>
T* DefaultFactory() {
  return new T();
}

}  // namespace Knot

#endif  // KNOT_FACTORY_HPP
