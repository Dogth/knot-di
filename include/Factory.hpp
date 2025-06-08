/** @file Factory.hpp
 * @brief Заголовочный файл для класса Factory. Класс предназначен для создания
 * и уничтожения экземпляров сервисов.
 * @version 1.0
 *
 * Этот файл содержит определение интерфейса IFactory и шаблонного класса
 * Factory, который реализует создание и уничтожение экземпляров сервисов.
 */
#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "ContainerMacros.hpp"
namespace Knot {
/** @brief Интерфейс для фабрик, создающих и уничтожающих экземпляры сервисов
 * @details Этот интерфейс определяет методы для создания и уничтожения
 * экземпляров сервисов. Он используется для абстракции процесса создания
 * сервисов в контейнере Knot.
 */
class IFactory {
public:
  virtual ~IFactory() {};
  virtual void *create(void *buffer) = 0;
  virtual void destroy(void *instance) = 0;
};

/** @brief Фабрика для создания и уничтожения экземпляров сервисов
 * @details Этот шаблонный класс реализует интерфейс IFactory и предоставляет
 * методы для создания и уничтожения экземпляров сервисов. Он может быть
 * использован для создания сервисов с различными параметрами конструктора.
 * @tparam T Тип сервиса, который будет создан фабрикой.
 *
 * @note Фабрика использует placement new для создания экземпляров сервисов в
 * заданном буфере. Если буфер не задан, используется стандартный new для
 * создания экземпляра.
 *
 * @note Похожие классы генерации фабрик создаются с помощью макроса
 * FACTORY_GEN, который позволяет создавать фабрики с различным количеством
 * аргументов.
 */
template <typename T> class Factory : public IFactory {
public:
  void *create(void *buffer) { return buffer ? new (buffer) T() : new T(); }
  void destroy(void *instance) {
    if (instance)
      static_cast<T *>(instance)->~T();
  }
};

FACTORY_GEN // Макрос для генерации фабрик с различной арностью

}; // namespace Knot

#endif // FACTORY_HPP
