/** @file Descriptor.hpp
 * @brief Заголовочный файл для структуры Descriptor. Структура предназначена
 * для хранения информации о сервисах, включая фабрику, стратегию создания и
 * экземпляр.
 * @version 1.0
 *
 * Этот файл содержит определение структуры Descriptor, которая используется для
 * хранения информации о сервисах в контейнере Knot.
 */
#ifndef DESCRIPTOR_HPP
#define DESCRIPTOR_HPP

#include "Factory.hpp"
#include "Strategy.hpp"

namespace Knot {
/** @brief Структура Descriptor для хранения информации о сервисах
 * @details Эта структура используется для хранения информации о сервисах,
 * включая фабрику, стратегию создания, экземпляр и хранилище.
 */
struct Descriptor {
  IFactory *factory; // Указатель на фабрику, которая создает экземпляры сервиса
  Strategy strategy; // Стратегия создания сервиса (SINGLETON или TRANSIENT)
  void *instance; // Указатель на экземпляр сервиса, если он создан. Применяется
                  // только для SINGLETON
  void *storage;  // Указатель на хранилище, где хранится сервис. Используется
                  // для SINGLETON сервисов

  Descriptor() : factory(0), strategy(), instance(0), storage(0) {}

private:
  Descriptor &operator=(const Descriptor &); // Запрет присваивания дескриптора
  Descriptor(const Descriptor &);            // Запрет копирования дескриптора
};
}; // namespace Knot

#endif // DESCRIPTOR_HPP
