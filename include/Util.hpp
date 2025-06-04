#ifndef UTIL_HPP
#define UTIL_HPP

#include "Descriptor.hpp"

namespace Knot {
template <typename T> void *TypeId() {
  static char id;
  return &id;
}

struct TransientInfo {
  void *ptr;
  IFactory *factory;
};

struct RegistryEntry {
  void *type_id;
  Descriptor desc;
};
}; // namespace Knot

#endif // TYPE_ID_HPP
