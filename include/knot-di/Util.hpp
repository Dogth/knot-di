/** @file Util.hpp
 * @brief Заголовочный файл для вспомогательных функций и структур
 * @version 1.0
 *
 * Этот файл содержит вспомогательные функции и структуры, используемые в
 * контейнере Knot. Он включает в себя функции для получения идентификаторов
 * типов и структуры для хранения информации о временных сервисах.
 */
#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstddef>

#include "Descriptor.hpp"

namespace Knot {
/** @brief Функция для получения уникального идентификатора типа
 * @details Эта функция используется для получения уникального идентификатора
 * типа T. Она возвращает указатель на статическую переменную, которая будет
 * уникальной для каждого типа.
 *
 * @tparam T Тип, для которого нужно получить уникальный идентификатор.
 * @return Указатель на уникальный идентификатор типа T.
 */
template <typename T>
void* TypeId() {
  static char id;  // Статическая переменная, уникальная для каждого типа T
  return &id;      // Возвращаем указатель на уникальный идентификатор типа
}

/** @brief Структура для хранения информации о временных сервисах
 * @details Эта структура используется для хранения информации о временных
 * сервисах, включая указатель на экземпляр, фабрику, которая создает этот
 * экземпляр, и размер выделенной памяти.
 */
struct TransientInfo {
  void* ptr;          // Указатель на экземпляр временного сервиса
  IFactory* factory;  // Указатель на фабрику, которая создает этот экземпляр
  size_t alloc_size;  // Размер выделенной памяти для этого экземпляра
};

/** @brief Структура для хранения информации о сервисах в реестре
 * @details Эта структура используется для хранения информации о сервисах в
 * реестре, включая указатель на тип сервиса и дескриптор, содержащий информацию
 * о фабрике, стратегии создания и экземпляре.
 */
struct RegistryEntry {
  void* type;       // Указатель на уникальный идентификатор типа сервиса
  Descriptor desc;  // Дескриптор, содержащий информацию о сервисе
};
};  // namespace Knot

#endif  // TYPE_ID_HPP
