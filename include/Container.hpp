#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "ContainerMacros.hpp"
#include "Descriptor.hpp"
#include "Factory.hpp"
#include "MemoryPool.hpp"
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

class Container {
private:
  Container(const Container &);
  Container &operator=(const Container &);

  size_t _service_count;
  size_t _transient_count;

  MemoryPool _pool;

  RegistryEntry _registry[KNOT_MAX_SERVICES];
  TransientInfo _transients[KNOT_MAX_TRANSIENTS];

  RegistryEntry *find_entry(void *tid) {
    for (size_t i = 0; i < _service_count; ++i)
      if (_registry[i].type == tid)
        return &_registry[i];
    return nullptr;
  }

  template <typename T> bool register_singleton(IFactory *factory) {
    size_t size = sizeof(T);
    void *mem = _pool.allocate(size, sizeof(void *));
    if (!mem)
      return false;
    RegistryEntry &entry = _registry[_service_count++];
    entry.type = TypeId<T>();
    entry.desc.factory = factory;
    entry.desc.strategy = SINGLETON;
    entry.desc.storage = mem;
    entry.desc.instance = nullptr;
    return true;
  }

  template <typename T> bool register_transient(IFactory *factory) {
    RegistryEntry &entry = _registry[_service_count++];
    entry.type = TypeId<T>();
    entry.desc.factory = factory;
    entry.desc.strategy = TRANSIENT;
    entry.desc.storage = nullptr;
    entry.desc.instance = nullptr;
    return true;
  }

  template <typename T>
  inline bool addService(Strategy strategy, IFactory *factory) {
    void *tid = TypeId<T>();
    if (!factory || _service_count >= KNOT_MAX_SERVICES || find_entry(tid))
      return false;
    switch (strategy) {
    case SINGLETON:
      return register_singleton<T>(factory);
      break;
    case TRANSIENT:
      return register_transient<T>(factory);
      break;
    default:
      return false;
    }
  }

public:
  // WARN: allocates on heap if no buffer is provided

  Container() : _service_count(0), _transient_count(0), _pool(4096) {}
  explicit Container(size_t max_bytes)
      : _service_count(0), _transient_count(0), _pool(max_bytes) {}
  Container(void *buffer, size_t buffer_size)
      : _service_count(0), _transient_count(0), _pool(buffer, buffer_size) {}
  Container(void *buffer, size_t buffer_size, size_t offset)
      : _service_count(0), _transient_count(0), _pool(buffer, buffer_size) {
    _pool.allocate(buffer_size - offset, 1);
  }

  ~Container() {
    destroyAllTransients();
    destroyAllSingletons();
  }

  template <typename T> bool registerService(Strategy strategy) {
    return addService<T>(strategy, new Factory<T>());
  }

  REGISTER_GEN

  template <typename T> T *resolve() {
    KNOT_INFO("Resolving service with ID: {}", TypeId<T>());
    RegistryEntry *entry = find_entry(TypeId<T>());
    if (!entry)
      return nullptr;
    Descriptor &desc = entry->desc;
    switch (desc.strategy) {
    case SINGLETON: {
      if (!desc.instance)
        desc.instance = desc.factory->create(desc.storage);
      return static_cast<T *>(desc.instance);
    }
    case TRANSIENT: {
      if (_transient_count >= KNOT_MAX_TRANSIENTS)
        return nullptr;
      size_t size = 0;
      KNOT_ERR("Allocating transient service with ID: {}", TypeId<T>());
      void *mem = _pool.allocate(sizeof(T), sizeof(void *), &size);
      if (!mem)
        return nullptr;
      T *ptr = static_cast<T *>(desc.factory->create(mem));
      _transients[_transient_count].factory = desc.factory;
      _transients[_transient_count].ptr = ptr;
      _transients[_transient_count].alloc_size = size;
      ++_transient_count;
      return ptr;
    }
    default:
      return nullptr;
    }
  }

  void destroyAllSingletons() {
    for (size_t i = 0; i < _service_count; ++i) {
      Descriptor &desc = _registry[i].desc;
      if (desc.strategy == SINGLETON && desc.instance) {
        desc.factory->destroy(desc.instance);
        desc.instance = nullptr;
      }
    }
  }
  void destroyAllTransients() {
    for (size_t i = 0; i < _transient_count; ++i) {
      if (_transients[i].ptr && _transients[i].factory)
        _transients[i].factory->destroy(_transients[i].ptr);
      if (_transients[i].ptr)
        _pool.deallocate(_transients[i].ptr, _transients[i].alloc_size);
      _transients[i].ptr = nullptr;
      _transients[i].alloc_size = 0;
      _transients[i].factory = nullptr;
    }
    _transient_count = 0;
  }

  template <typename T> void destroyTransient(T *ptr) {
    for (size_t i = 0; i < _transient_count; ++i) {
      if (_transients[i].ptr == ptr) {
        _transients[i].factory->destroy(ptr);
        _pool.deallocate(_transients[i].ptr, _transients[i].alloc_size);
        _transients[i].ptr = nullptr;
        _transients[i].alloc_size = 0;
        _transients[i].factory = nullptr;
        for (size_t j = i + 1; j < _transient_count; ++j)
          _transients[j - 1] = _transients[j];
        --_transient_count;
        break;
      }
    }
  }
};
} // namespace Knot

#endif // CONTAINER_HPP
