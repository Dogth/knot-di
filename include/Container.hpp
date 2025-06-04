#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "ContainerMacros.hpp"
#include "Descriptor.hpp"
#include "IFactory.hpp"
#include "StaticMap.hpp"
#include <typeinfo>

// Container class for managing service registrations and resolutions
namespace Knot {

FACTORY_GEN // PERF: vtable lookups, but idk how to avoid them here
            // PERF: plus like separate class for each arity :/
    template <typename T>
    class Factory : public IFactory {
public:
  virtual void *create() override { return new T(); }
  virtual void deleteInstance(void *instance) override {
    delete static_cast<T *>(instance);
  }
};

class Container {
private:
#ifdef KNOT_SIZE
  typedef knot::static_map<const std::type_info *, Descriptor *, KNOT_SIZE>
      Registry;
#else
  typedef StaticMap<const std::type_info *, Descriptor *, 16> Registry;
#endif
  Registry _registry;

  static Container *_instancePtr;

  Container() = default;                            // no-args constructor
  Container(const Container &) = delete;            // uncopyable
  Container &operator=(const Container &) = delete; // unmovable

public:
  template <typename T> void registerService(Strategy strategy) {
    if (_registry.find(&typeid(T))) {
      return; // already registered
    }
    _registry.insert(&typeid(T), new Descriptor(new Factory<T>(), strategy));
  }

  // All .registerService declarations are preprocessed by macros
  // Need to use '--ffunction-sections -fdata-sections' with compiler
  // and '-Wl --gc-sections' for linker
  REGISTER_GEN

  static Container &instance() {
    static Container instance;
    return instance;
  }

  // FIXME: gets optimized out by compiler
  /*
          static Container &instance() {
      if (!_instancePtr) {
        _instancePtr = new Container();
      }
      return *_instancePtr;
    }
  */

  ~Container() {
    for (Registry::size_type i = 0; i < _registry.size(); ++i) {
      _registry.begin()[i].second->factory->deleteInstance(
          _registry.begin()[i].second->instance);
    }
  }

  // TODO: bless this mess
  template <typename T> T *resolve() {
    Descriptor **entry = _registry.find(&typeid(T));
    if (!entry) {
      return nullptr;
    }

    if ((*entry)->strategy == Strategy::SINGLETON) {
      if (!(*entry)->instance) {
        (*entry)->instance = (*entry)->factory->create();
      }
      return static_cast<T *>((*entry)->instance);
    } else {
      return static_cast<T *>((*entry)->factory->create());
    }
  }
};
} // namespace Knot

#endif // CONTAINER_HPP
