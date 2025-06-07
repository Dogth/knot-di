#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "ContainerMacros.hpp"
namespace Knot {
class IFactory {
public:
  virtual ~IFactory() {};
  virtual void *create(void *buffer) = 0;
  virtual void destroy(void *instance) = 0;
};

template <typename T> class Factory : public IFactory {
public:
  void *create(void *buffer) { return buffer ? new (buffer) T() : new T(); }
  void destroy(void *instance) {
    if (instance)
      static_cast<T *>(instance)->~T();
  }
};

FACTORY_GEN

}; // namespace Knot

#endif // FACTORY_HPP
