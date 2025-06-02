#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <typeinfo>

#define TEMPLATE_ARGS_1 , typename A1
#define FUNC_ARGS_1 , A1 arg1
#define CLASS_PARAMS A1 _arg1
#define CONSTR_ARGS_1 _arg1(arg1)

#define FACTORY_GEN(N) FACTORY_##N##_GEN()

/// Arity macro for generating factory classes based on the number of
/// parameters.
#define FACTORY_0_GEN()                                                        \
  namespace Knot {                                                             \
  template <typename T> class Factory0 : public IFactory {                     \
  public:                                                                      \
    Factory0() {}                                                              \
    virtual void *create() { return new T(); }                                 \
    virtual const std::type_info &getTypeInfo() const { return typeid(T); }    \
    virtual void deleteInstance(void *instance) {                              \
      delete static_cast<T *>(instance);                                       \
    }                                                                          \
  };                                                                           \
  }

#define FACTORY_1_GEN()                                                        \
  namespace Knot {                                                             \
  template <typename T, typename A1> class Factory1 : public IFactory {        \
    A1 _arg1;                                                                  \
                                                                               \
  public:                                                                      \
    Factory1(A1 arg1) : _arg1(arg1) {}                                         \
    virtual void *create() { return new T(_arg1); }                            \
    virtual const std::type_info &getTypeInfo() const { return typeid(T); }    \
    virtual void deleteInstance(void *instance) {                              \
      delete static_cast<T *>(instance);                                       \
    }                                                                          \
  };                                                                           \
  }

#define FACTORY_2_GEN()                                                        \
  namespace Knot {                                                             \
  template <typename T, typename A1, typename A2>                              \
  class Factory1 : public IFactory {                                           \
    A1 _arg1;                                                                  \
    A2 _arg2;                                                                  \
                                                                               \
  public:                                                                      \
    Factory2(A1 arg1, A2 arg2) : _arg1(arg1), _arg2(arg2) {}                   \
    virtual void *create() { return new T(_arg1, _arg2); }                     \
    virtual const std::type_info &getTypeInfo() const { return typeid(T); }    \
    virtual void deleteInstance(void *instance) {                              \
      delete static_cast<T *>(instance);                                       \
    }                                                                          \
  };                                                                           \
  }

// Factory interface
namespace Knot {
class IFactory {
public:
  virtual ~IFactory() {};
  virtual void *create() = 0;
  virtual const std::type_info &getTypeInfo() const = 0;
  virtual void deleteInstance(void *instance) = 0;
};
}; // namespace Knot

FACTORY_GEN(1)

#endif // FACTORY_HPP
