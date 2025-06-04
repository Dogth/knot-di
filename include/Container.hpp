#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "ContainerMacros.hpp"
#include "Descriptor.hpp"
#include "IFactory.hpp"
#include "Util.hpp"
#include <cstddef>

#ifndef KNOT_MAX_SERVICES
#define KNOT_MAX_SERVICES 16
#endif

#ifndef KNOT_MAX_TRANSIENTS
#define KNOT_MAX_TRANSIENTS 32
#endif

// Container class for managing service registrations and resolutions
namespace Knot {
// TODO: update macros to use placement new
FACTORY_GEN // PERF: vtable lookups, but idk how to avoid them here
            // PERF: plus like separate class for each arity :/
            //
    template <typename T>
    class Factory : public IFactory {
public:
  void *create(void *buffer) { return buffer ? new (buffer) T() : new T(); }
  void destroy(void *instance) {
    if (instance)
      static_cast<T *>(instance)->~T();
  }
};

class Container {
private:
  Container(const Container &);
  Container &operator=(const Container &);

  RegistryEntry _registry[KNOT_MAX_SERVICES];
  size_t _count;

  TransientInfo _transients[KNOT_MAX_TRANSIENTS];
  size_t _transient_count;

  template <typename T> static void *_singleton_storage() {
    static union {
      char data[sizeof(T)];
      long double align;
    } storage;
    return &storage;
  }

  RegistryEntry *find_entry(void *tid) {
    for (size_t i = 0; i < _count; ++i)
      if (_registry[i].type_id == tid)
        return &_registry[i];
    return 0;
  }

public:
  Container() : _count(0) {}

  // Core function for generation. Arity is set by X-macros
  template <typename T> bool registerService(Strategy strategy) {
    if (_count >= KNOT_MAX_SERVICES)
      return false;
    void *tid = TypeId<T>();
    if (find_entry(tid))
      return false;
    RegistryEntry &entry = _registry[_count++];
    entry.type_id = tid;
    entry.desc.factory = new Factory<T>();
    entry.desc.strategy = strategy;
    entry.desc.storage = (strategy == SINGLETON) ? _singleton_storage<T>() : 0;
    entry.desc.instance = 0;
    if (strategy == TRANSIENT) {
      _transients[_transient_count].ptr = entry.desc.factory->create(0);
      _transients[_transient_count].factory = entry.desc.factory;
      ++_transient_count;
    }
    return true;
  }

  template <typename T> T *resolve() {
    RegistryEntry *entry = find_entry(TypeId<T>());
    if (!entry)
      return 0;
    Descriptor &desc = entry->desc;
    if (desc.strategy == SINGLETON) {
      if (!desc.instance)
        desc.instance = desc.factory->create(desc.storage);
      return static_cast<T *>(desc.instance);
    } else {
      if (_transient_count >= KNOT_MAX_TRANSIENTS)
        return 0;
      T *ptr = static_cast<T *>(desc.factory->create(0));
      _transients[_transient_count].ptr = ptr;
      _transients[_transient_count].factory = desc.factory;
      ++_transient_count;
      return ptr;
    }
  }

  void destroyAllTransients() {
    for (size_t i = 0; i < _transient_count; ++i) {
      _transients[i].factory->destroy(_transients[i].ptr);
      _transients[i].ptr = 0;
    }
    _transient_count = 0;
  }

  template <typename T> void destroyTransient(T *ptr) {
    for (size_t i = 0; i < _transient_count; ++i) {
      if (_transients[i].ptr == ptr) {
        _transients[i].factory->destroy(ptr);
        for (size_t j = i + 1; j < _transient_count; ++j)
          _transients[j - 1] = _transients[j];
        --_transient_count;
        return;
      }
    }
  }
};
} // namespace Knot

#endif // CONTAINER_HPP
