#ifndef CONTAINER_MACROS_HPP
#define CONTAINER_MACROS_HPP

// Tuple expansion macro
#define EXPAND(...) __VA_ARGS__

// ------ Service Macros ------
#define R_ARITY_LIST(X)                                                        \
  X(1, (typename A1), (A1 arg1), (A1), (arg1))                                 \
  X(2, (typename A1, typename A2), (A1 arg1, A2 arg2), (A1, A2), (arg1, arg2)) \
  X(3, (typename A1, typename A2, typename A3), (A1 arg1, A2 arg2, A3 arg3),   \
    (A1, A2, A3), (arg1, arg2, arg3))                                          \
  X(4, (typename A1, typename A2, typename A3, typename A4),                   \
    (A1 arg1, A2 arg2, A3 arg3, A4 arg4), (A1, A2, A3, A4),                    \
    (arg1, arg2, arg3, arg4))

#define R_GEN(N, TMPL, FUNC, TPS, ARGS)                                        \
  template <typename T, EXPAND TMPL>                                           \
  void registerService(Strategy strategy, EXPAND FUNC) {                       \
    addService<T>(strategy, new Factory##N<T, EXPAND TPS>(EXPAND ARGS));       \
  }

#define REGISTER_GEN R_ARITY_LIST(R_GEN)

// ------- Factory Macros -------
// Arity list can(and probably should) be redefined
#define F_ARITY_LIST(X)                                                        \
  X(1, (typename A1), (A1 _arg1), (A1 arg1), (_arg1(arg1)), (_arg1))           \
  X(2, (typename A1, typename A2), (A1 _arg1; A2 _arg2), (A1 arg1, A2 arg2),   \
    (_arg1(arg1), _arg2(arg2)), (_arg1, _arg2))                                \
  X(3, (typename A1, typename A2, typename A3),                                \
    (A1 _arg1; A2 _arg2; A3 _arg3), (A1 arg1, A2 arg2, A3 arg3),               \
    (_arg1(arg1), _arg2(arg2), _arg3(arg3)), (_arg1, _arg2, _arg3))            \
  X(4, (typename A1, typename A2, typename A3, typename A4),                   \
    (A1 _arg1; A2 _arg2; A3 _arg3; A4 _arg4),                                  \
    (A1 arg1, A2 arg2, A3 arg3, A4 arg4),                                      \
    (_arg1(arg1), _arg2(arg2), _arg3(arg3), _arg4(arg4)),                      \
    (_arg1, _arg2, _arg3, _arg4))

#define F_GEN(N, TMPL, FIELDS, ARGS, CONSTR, CREATE)                           \
  template <typename T, EXPAND TMPL> class Factory##N : public IFactory {      \
    EXPAND FIELDS;                                                             \
                                                                               \
  public:                                                                      \
    Factory##N(EXPAND ARGS) : EXPAND CONSTR {}                                 \
    virtual void *create(void *buffer) {                                       \
      return buffer ? new (buffer) T(EXPAND CREATE) : new T(EXPAND CREATE);    \
    }                                                                          \
    void destroy(void *instance) {                                             \
      if (instance)                                                            \
        static_cast<T *>(instance)->~T();                                      \
    }                                                                          \
  };

#define FACTORY_GEN F_ARITY_LIST(F_GEN)

#endif // CONTAINER_MACROS_HPP
