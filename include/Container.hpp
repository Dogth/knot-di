#ifndef CONTAINER_HPP
#define CONTAINER_HPP

// TODO: placement new
// TODO: error handling
//

#include "Descriptor.hpp"
#include "IFactory.hpp"
#include "TypeComparator.hpp"
#include <map>

// Nullary
#define TEMPLATE_ARGS_0
#define FUNC_ARGS_0
#define FACTORY_TYPES_0
#define FACTORY_ARGS_0

// Unary
#define TEMPLATE_ARGS_1 , typename A1
#define FUNC_ARGS_1 , A1 arg1
#define FACTORY_TYPES_1 , A1
#define FACTORY_ARGS_1 arg1

// Binary
#define TEMPLATE_ARGS_2 typename A1, typename A2
#define FUNC_ARGS_2 A1 arg1, A2 arg2
#define FACTORY_TYPES_2 A1, A2
#define FACTORY_ARGS_2 arg1, arg2

// Macro for registering services
#define REGISTER_SERVICE_GEN(N)                                                \
  template <typename IT, typename T TEMPLATE_ARGS_##N>                         \
  void registerService(Strategy strategy FUNC_ARGS_##N) {                      \
    if (_registry.count(&typeid(IT))) {                                        \
      return;                                                                  \
    }                                                                          \
    _registry[&typeid(IT)] = new Descriptor(                                   \
        new Factory##N<T FACTORY_TYPES_##N>(FACTORY_ARGS_##N), strategy);      \
  }

// Container class for managing service registrations and resolutions
namespace Knot {
class Container {
private:
  typedef std::map<const std::type_info *, Descriptor *, TypeComparator>
      Registry;
  Registry _registry;

  Container() = default;
  Container(const Container &) = delete;
  Container &operator=(const Container &) = delete;

public:
  static Container &instance() {
    static Container instance;
    return instance;
  }

  ~Container() {
    for (Registry::iterator it = _registry.begin(); it != _registry.end();
         ++it) {
      delete it->second;
    }
    _registry.clear();
  }

  // TODO: #define KNOT_PARAMS_MAX registration limit
  // WARN: Must be mover outside and controlled properly
  REGISTER_SERVICE_GEN(1);
  REGISTER_SERVICE_GEN(0);

  template <typename T> T *resolve() {
    Registry::iterator it = _registry.find(&typeid(T));
    if (it == _registry.end()) {
      // FIXME: error handling
      return 0;
    };

    Descriptor *entry = it->second;
    if (entry->strategy == Strategy::SINGLETON) {
      if (!entry->instance) {
        entry->instance = entry->factory->create();
        if (!entry->instance) {
          // FIXME: error handling
          return 0;
        }
      }
      return static_cast<T *>(entry->instance);
    } else {
      void *newInstance = entry->factory->create();
      if (!newInstance) {
        // FIXME: error handling
        return 0;
      }
      return static_cast<T *>(newInstance);
    }
  }
};
} // namespace Knot

#endif // CONTAINER_HPP
