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

FACTORY_GEN

template <typename T> class Factory : public IFactory {
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
  size_t _used_bytes;
  size_t _max_bytes;
  void *_buffer;
  size_t _buffer_offset;

  TransientInfo _transients[KNOT_MAX_TRANSIENTS];
  size_t _transient_count;

  void *_allocate(size_t size, size_t align, size_t *out_alloc_size = 0) {
    if (_buffer) {
      KNOT_INFO("Allocating {} bytes from buffer at offset {}", size,
                _buffer_offset);
      size_t space = _max_bytes - _buffer_offset;
      char *base = static_cast<char *>(_buffer) + _buffer_offset;
      size_t misalign = reinterpret_cast<size_t>(base) % align;
      size_t pad = misalign ? (align - misalign) : 0;
      if (space < size + pad) {
        KNOT_ERR(
            "Buffer overflow: {} bytes requested, but only {} bytes available",
            size + pad, space);
        return 0;
      }
      void *ptr = base + pad;
      if (out_alloc_size)
        *out_alloc_size = size + pad;
      _buffer_offset += size + pad;
      _used_bytes += size + pad;
      return ptr;
    } else {
      if (_used_bytes + size > _max_bytes) {
        KNOT_ERR(
            "Heap overflow: {} bytes requested, but only {} bytes available",
            size, _max_bytes - _used_bytes);
        return 0;
      } else if (size == 0)
        return nullptr; // Avoid allocating zero bytes
      KNOT_INFO("Allocating {} bytes from heap", size);
      void *ptr = operator new(size);
      if (ptr) {
        if (out_alloc_size)
          *out_alloc_size = size;
        _used_bytes += size;
      }
      return ptr;
    }
  }
  void _deallocate(void *ptr, size_t size) {
    if (!_buffer && ptr) {
      operator delete(ptr);
    }
    _used_bytes -= size;
  }

  RegistryEntry *find_entry(void *tid) {
    KNOT_INFO("Looking for service with ID: {} s[{}/{}] buf: {}", tid, _count,
              KNOT_MAX_SERVICES, _used_bytes);
    for (size_t i = 0; i < _count; ++i)
      if (_registry[i].type == tid) {
        KNOT_INFO("Found service with ID: {}", tid);
        return &_registry[i];
      } else
        KNOT_WARN("Service with ID {} not found", tid);
    return 0;
  }

  template <typename T> bool register_singleton(IFactory *factory) {
    size_t size = sizeof(T);
    void *storage = _allocate(size, alignof(T));
    ;
    if (!storage)
      return false;
    RegistryEntry &entry = _registry[_count++];
    entry.type = TypeId<T>();
    entry.desc.factory = factory;
    entry.desc.strategy = SINGLETON;
    entry.desc.storage = storage;
    entry.desc.instance = nullptr;
    return true;
  }

  template <typename T> bool register_transient(IFactory *factory) {
    RegistryEntry &entry = _registry[_count++];
    entry.type = TypeId<T>();
    entry.desc.factory = factory;
    entry.desc.strategy = TRANSIENT;
    entry.desc.storage = nullptr;
    entry.desc.instance = nullptr;
    return true;
  }
  // TODO: rewrite using .registerSingleton and .registerTransient
  template <typename T>
  inline bool addService(Strategy strategy, IFactory *factory) {
    void *tid = TypeId<T>();
    if (!factory) {
      KNOT_WARN("Factory is null. Cannot register service with ID: {}", tid);
      return false;
    }
    if (strategy != SINGLETON && strategy != TRANSIENT) {
      KNOT_WARN("Invalid strategy: {}. Must be SINGLETON or TRANSIENT",
                static_cast<int>(strategy));
      return false;
    }
    KNOT_INFO("Registering service with ID: {}", tid);
    if (_count >= KNOT_MAX_SERVICES) {
      KNOT_WARN("Maximum service count reached: {}. Skipping",
                KNOT_MAX_SERVICES);
      return false;
    }
    if (find_entry(tid)) {
      KNOT_WARN("Type ID: {} is already registered. Skipping", tid);
      return false;
    }
    if (strategy == SINGLETON) {
      KNOT_INFO("Registering ID: {} as Singleton");
      register_singleton<T>(factory);
    } else {
      register_transient<T>(factory);
    }
    return true;
  }

public:
  // WARN: allocates on heap if no buffer is provided
  Container()
      : _count(0), _used_bytes(0), _max_bytes(4096), _buffer(nullptr),
        _buffer_offset(0), _transient_count(0) {}

  explicit Container(size_t max_bytes)
      : _count(0), _used_bytes(0), _max_bytes(max_bytes), _buffer(nullptr),
        _buffer_offset(0), _transient_count(0) {}

  Container(void *buffer, size_t buffer_size)
      : _count(0), _used_bytes(0), _max_bytes(buffer_size), _buffer(buffer),
        _buffer_offset(0), _transient_count(0) {}

  Container(void *buffer, size_t buffer_size, size_t offset)
      : _count(0), _used_bytes(0), _max_bytes(buffer_size), _buffer(buffer),
        _buffer_offset(offset), _transient_count(0) {}

  template <typename T> bool registerService(Strategy strategy) {
    return addService<T>(strategy, new Factory<T>());
  }

  REGISTER_GEN

  template <typename T> T *resolve() {
    KNOT_INFO("Resolving service with ID: {}", TypeId<T>());
    RegistryEntry *entry = find_entry(TypeId<T>());
    if (!entry) {
      KNOT_WARN("Service with ID: {} is not registered. Returning null",
                TypeId<T>());
      return nullptr;
    }
    Descriptor &desc = entry->desc;
    if (desc.strategy == SINGLETON) {
      KNOT_INFO("Found singleton service with ID: {}", TypeId<T>());
      if (!desc.instance)
        KNOT_INFO("Creating singleton instance for service with ID: {}",
                  TypeId<T>());
      desc.instance = desc.factory->create(desc.storage);
      return static_cast<T *>(desc.instance);
    } else {
      KNOT_INFO("Found transient service: {}", TypeId<T>());
      if (_transient_count >= KNOT_MAX_TRANSIENTS) {
        KNOT_WARN("Transient service count exceeded: [{}/{}]. Returning null",
                  _transient_count, KNOT_MAX_TRANSIENTS);
        return nullptr;
      }
      size_t size = 0;
      void *mem = _allocate(sizeof(T), sizeof(void *), &size);
      if (!mem)
        return nullptr;
      T *ptr = static_cast<T *>(desc.factory->create(mem));
      _transients[_transient_count].factory = desc.factory;
      _transients[_transient_count].ptr = ptr;
      _transients[_transient_count].alloc_size = size;
      ++_transient_count;
      KNOT_INFO("Creating new transient instance for service with ID: {} "
                "[{}/{}] ADDR: {}",
                TypeId<T>(), _transient_count, KNOT_MAX_TRANSIENTS,
                static_cast<void *>(ptr));
      return ptr;
    }
  }
  void destroyAllSingletons() {
    KNOT_INFO("Destroying all singletons [{}]", _count);
    for (size_t i = 0; i < _count; ++i) {
      Descriptor &desc = _registry[i].desc;
      if (desc.strategy == SINGLETON && desc.instance) {
        desc.factory->destroy(desc.instance);
        KNOT_INFO("Destroyed singleton service with ID: {}", _registry[i].type);
        _deallocate(desc.instance, sizeof(desc.instance));
        desc.instance = 0;
      }
    }
  }

  void destroyAllTransients() {
    KNOT_INFO("Destroying all transient services [{}/{}]", _transient_count,
              KNOT_MAX_TRANSIENTS);
    for (size_t i = 0; i < _transient_count; ++i) {
      if (_transients[i].ptr && _transients[i].factory)
        _transients[i].factory->destroy(_transients[i].ptr);
      _deallocate(_transients[i].ptr, _transients[i].alloc_size);
      KNOT_INFO("Destroyed transient service with at ADDR: {} [{}]",
                _transients[i].ptr, _transients[i].alloc_size);
      _transients[i].ptr = nullptr;
      _transients[i].alloc_size = 0;
      _transients[i].factory = nullptr;
    }
    _transient_count = 0;
  }

  template <typename T> void destroyTransient(T *ptr) {
    KNOT_INFO("Destroying transient service with ID: {} at ADDR: {}",
              TypeId<T>(), static_cast<void *>(ptr));
    for (size_t i = 0; i < _transient_count; ++i) {
      if (_transients[i].ptr == ptr) {
        _transients[i].factory->destroy(ptr);
        _deallocate(_transients[i].ptr, _transients[i].alloc_size);
        KNOT_INFO("Deleted Transient with ID: {}. Shifting other transients",
                  TypeId<T>());
        for (size_t j = i + 1; j < _transient_count; ++j)
          _transients[j - 1] = _transients[j];
        --_transient_count;
        return;
      }
    }
  }

  size_t used_bytes() const { return _used_bytes; }
  size_t max_bytes() const { return _max_bytes; }
  size_t remaining_bytes() const { return _max_bytes - _used_bytes; }
};
} // namespace Knot

#endif // CONTAINER_HPP
