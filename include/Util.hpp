#ifndef UTIL_HPP
#define UTIL_HPP

#include "Descriptor.hpp"
#include <cstddef>

namespace Knot {
template <typename T> void *TypeId() {
  static char id;
  return &id;
}

struct TransientInfo {
  void *ptr;
  IFactory *factory;
  size_t alloc_size;
};

struct RegistryEntry {
  void *type;
  Descriptor desc;
};
}; // namespace Knot

#endif // TYPE_ID_HPP
