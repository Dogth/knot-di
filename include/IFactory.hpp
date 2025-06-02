#ifndef FACTORY_HPP
#define FACTORY_HPP

#include <typeinfo>

// TODO: simplify
//  Nullary
#define FACTORY_TEMPLATE_ARGS_0
#define FACTORY_FIELDS_0
#define FACTORY_FUNCTION_ARGS_0
#define FACTORY_CONSTR_ARGS_0
#define FACTORY_CREATE_ARGS_0

// Unary
#define FACTORY_TEMPLATE_ARGS_1 , typename A1
#define FACTORY_FIELDS_1 A1 _arg1
#define FACTORY_FUNCTION_ARGS_1 A1 arg1
#define FACTORY_CONSTR_ARGS_1 : _arg1(arg1)
#define FACTORY_CREATE_ARGS_1 _arg1

// Binary
#define FACTORY_TEMPLATE_ARGS_2 typename A1, typename A2
#define FACTORY_FIELDS_2 A1 _arg1, A2 _arg2
#define FACTORY_FUNCTION_ARGS_2 A1 arg1, A2 arg2
#define FACTORY_CONSTR_ARGS_2 _arg1(arg1), _arg2(arg2)
#define FACTORY_CREATE_ARGS_2 _arg1, _arg2

#define FACTORY_GEN(N)                                                         \
  namespace Knot {                                                             \
  template <typename T FACTORY_TEMPLATE_ARGS_##N>                              \
  class Factory##N : public IFactory {                                         \
    FACTORY_FIELDS_##N;                                                        \
                                                                               \
  public:                                                                      \
    Factory##N(FACTORY_FUNCTION_ARGS_##N) FACTORY_CONSTR_ARGS_##N {}           \
    virtual void *create() { return new T(FACTORY_CREATE_ARGS_##N); }          \
    virtual const std::type_info &getTypeInfo() const { return typeid(T); }    \
    virtual void deleteInstance(void *instance) {                              \
      delete static_cast<T *>(instance);                                       \
    }                                                                          \
  };                                                                           \
  };
// C13 & D10

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

FACTORY_GEN(1);
FACTORY_GEN(0);

#endif // FACTORY_HPP
