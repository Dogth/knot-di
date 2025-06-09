/** @file ContainerMacros.hpp
 * @brief Макросы для работы с контейнерами и сервисами
 * @version 1.0
 *
 * Этот файл содержит макросы, используемые для регистрации сервисов и создания
 * фабрик. Макросы позволяют легко управлять количеством аргументов и создавать
 * шаблоны кода.
 */

#ifndef CONTAINER_MACROS_HPP
#define CONTAINER_MACROS_HPP

// Tuple expansion macro
#define EXPAND(...) __VA_ARGS__ // Макрос для разворачивания аргументов

// ------ Service Macros ------
/** @brief Макрос для регистрации сервисов с различным количеством аргументов
 * @details Этот макрос используется для создания шаблонов функций регистрации
 * сервисов с различным количеством аргументов. Он позволяет легко добавлять
 * новые сервисы с разным количеством параметров.
 */
#define R_ARITY_LIST(X)                                                        \
  X(1, (typename A1), (A1 arg1), (A1), (arg1))                                 \
  X(2, (typename A1, typename A2), (A1 arg1, A2 arg2), (A1, A2), (arg1, arg2)) \
  X(3, (typename A1, typename A2, typename A3), (A1 arg1, A2 arg2, A3 arg3),   \
    (A1, A2, A3), (arg1, arg2, arg3))                                          \
  X(4, (typename A1, typename A2, typename A3, typename A4),                   \
    (A1 arg1, A2 arg2, A3 arg3, A4 arg4), (A1, A2, A3, A4),                    \
    (arg1, arg2, arg3, arg4))                                                  \
  X(5, (typename A1, typename A2, typename A3, typename A4, typename A5),      \
    (A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5), (A1, A2, A3, A4, A5),       \
    (arg1, arg2, arg3, arg4, arg5))                                            \
  X(6,                                                                         \
    (typename A1, typename A2, typename A3, typename A4, typename A5,          \
     typename A6),                                                             \
    (A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6),                    \
    (A1, A2, A3, A4, A5, A6), (arg1, arg2, arg3, arg4, arg5, arg6))            \
  X(7,                                                                         \
    (typename A1, typename A2, typename A3, typename A4, typename A5,          \
     typename A6, typename A7),                                                \
    (A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7),           \
    (A1, A2, A3, A4, A5, A6, A7), (arg1, arg2, arg3, arg4, arg5, arg6, arg7))  \
  X(8,                                                                         \
    (typename A1, typename A2, typename A3, typename A4, typename A5,          \
     typename A6, typename A7, typename A8),                                   \
    (A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7, A8 arg8),  \
    (A1, A2, A3, A4, A5, A6, A7, A8),                                          \
    (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8))

/** @brief Макрос для регистрации сервисов с различным количеством аргументов
 * @details Этот макрос используется для создания шаблонов функций регистрации
 * сервисов с различным количеством аргументов. Он позволяет легко добавлять
 * новые сервисы с разным количеством параметров.
 * @param N Номер арности
 * @param TMPL Шаблонные параметры
 * @param FUNC Функция регистрации
 * @param TPS Типы параметров
 * @param ARGS Аргументы для конструктора фабрики
 *
 * @note Расширение метода регистрации из @link Container::registerService для
 * различного количества аргументов.
 */
#define R_GEN(N, TMPL, FUNC, TPS, ARGS)                                        \
  template <typename T, EXPAND TMPL>                                           \
  bool registerService(Strategy strategy, EXPAND FUNC) {                       \
    uint8_t *mem =                                                             \
        _pool.allocate(sizeof(Factory##N<T, EXPAND TPS>), sizeof(void *));     \
    if (!mem)                                                                  \
      return false;                                                            \
    IFactory *factory = new (mem) Factory##N<T, EXPAND TPS>(EXPAND ARGS);      \
    if (_factory_count >= KNOT_MAX_SERVICES)                                   \
      return false;                                                            \
    _factories[_factory_count++] = factory;                                    \
    return addService<T>(strategy, factory);                                   \
  }

#define REGISTER_GEN                                                           \
  R_ARITY_LIST(R_GEN) // Макрос для генерации функций регистрации сервисов с
                      // различной арностью

// ------- Factory Macros -------
/** @brief Макросы для создания фабрик с различным количеством аргументов
 * @details Эти макросы используются для создания фабрик, которые могут
 * создавать объекты с различным количеством аргументов. Они позволяют легко
 * управлять количеством параметров и создавать шаблоны кода.
 */
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
    (_arg1, _arg2, _arg3, _arg4))                                              \
  X(5, (typename A1, typename A2, typename A3, typename A4, typename A5),      \
    (A1 _arg1; A2 _arg2; A3 _arg3; A4 _arg4; A5 _arg5),                        \
    (A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5),                             \
    (_arg1(arg1), _arg2(arg2), _arg3(arg3), _arg4(arg4), _arg5(arg5)),         \
    (_arg1, _arg2, _arg3, _arg4, _arg5))                                       \
  X(6,                                                                         \
    (typename A1, typename A2, typename A3, typename A4, typename A5,          \
     typename A6),                                                             \
    (A1 _arg1; A2 _arg2; A3 _arg3; A4 _arg4; A5 _arg5; A6 _arg6),              \
    (A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6),                    \
    (_arg1(arg1), _arg2(arg2), _arg3(arg3), _arg4(arg4), _arg5(arg5),          \
     _arg6(arg6)),                                                             \
    (_arg1, _arg2, _arg3, _arg4, _arg5, _arg6))                                \
  X(7,                                                                         \
    (typename A1, typename A2, typename A3, typename A4, typename A5,          \
     typename A6, typename A7),                                                \
    (A1 _arg1; A2 _arg2; A3 _arg3; A4 _arg4; A5 _arg5; A6 _arg6; A7 _arg7),    \
    (A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7),           \
    (_arg1(arg1), _arg2(arg2), _arg3(arg3), _arg4(arg4), _arg5(arg5),          \
     _arg6(arg6), _arg7(arg7)),                                                \
    (_arg1, _arg2, _arg3, _arg4, _arg5, _arg6, _arg7))                         \
  X(8,                                                                         \
    (typename A1, typename A2, typename A3, typename A4, typename A5,          \
     typename A6, typename A7, typename A8),                                   \
    (A1 _arg1; A2 _arg2; A3 _arg3; A4 _arg4; A5 _arg5; A6 _arg6; A7 _arg7;     \
     A8 _arg8),                                                                \
    (A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7, A8 arg8),  \
    (_arg1(arg1), _arg2(arg2), _arg3(arg3), _arg4(arg4), _arg5(arg5),          \
     _arg6(arg6), _arg7(arg7), _arg8(arg8)),                                   \
    (_arg1, _arg2, _arg3, _arg4, _arg5, _arg6, _arg7, _arg8))

/** @brief Макрос для создания фабрик с различным количеством аргументов
 * @details Этот макрос используется для создания шаблонов фабрик, которые могут
 * создавать объекты с различным количеством аргументов. Он позволяет легко
 * управлять количеством параметров и создавать шаблоны кода.
 * @param N Номер арности
 * @param TMPL Шаблонные параметры
 * @param FIELDS Поля класса фабрики
 * @param ARGS Аргументы конструктора фабрики
 * @param CONSTR Конструктор фабрики
 * @param CREATE Создание объекта
 *
 * @note Расширение метода создания фабрики для различного количества
 * аргументов. @link Factory::create
 */
#define F_GEN(N, TMPL, FIELDS, ARGS, CONSTR, CREATE)                           \
  template <typename T, EXPAND TMPL> class Factory##N : public IFactory {      \
    EXPAND FIELDS;                                                             \
                                                                               \
  public:                                                                      \
    Factory##N(EXPAND ARGS) : EXPAND CONSTR {}                                 \
    virtual void *create(uint8_t *buffer) {                                    \
      return buffer ? new (buffer) T(EXPAND CREATE) : new T(EXPAND CREATE);    \
    }                                                                          \
    void destroy(void *instance) {                                             \
      if (instance)                                                            \
        static_cast<T *>(instance)->~T();                                      \
    }                                                                          \
  };

#define FACTORY_GEN                                                            \
  F_ARITY_LIST(F_GEN) // Макрос для генерации фабрик с различной арностью

#endif // CONTAINER_MACROS_HPP
