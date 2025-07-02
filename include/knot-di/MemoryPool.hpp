/** @file MemoryPool.hpp
 * @brief Заголовочный файл для класса MemoryPool. Класс предназначен для
 * управления памятью с использованием пула памяти.
 * @version 1.0
 *
 * Этот файл содержит определение класса MemoryPool, который управляет памятью
 * с использованием пула памяти. Он включает в себя методы для выделения и
 * освобождения памяти, а также управления размером пула.
 */
#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <cstddef>
#include <cstdint>

#include "Util.hpp"
namespace Knot {
/** @brief Класс MemoryPool для управления памятью
 *
 * Этот класс предоставляет функциональность для управления памятью с
 * использованием пула памяти. Он позволяет выделять и освобождать память,
 * а также управлять размером пула.
 */
class MemoryPool {
 private:
  void* m_buffer;  // Указатель на буфер, используемый в качестве пула памяти

  size_t m_used_bytes;     // Количество использованных байт в пуле памяти
  size_t m_max_bytes;      // Максимальный размер пула памяти
  size_t m_buffer_offset;  // Смещение в буфере, где начинается следующий
                           // доступный блок памяти

 public:
  /** @brief Конструктор MemoryPool без параметров
   * @details Создает пул памяти с максимальным размером,
   * заданным в параметре max_bytes.
   *
   * @param max_bytes Максимальный размер пула памяти в байтах.
   */
  MemoryPool(size_t max_bytes)
      : m_buffer(NULL),
        m_used_bytes(0),
        m_max_bytes(max_bytes),
        m_buffer_offset(0) {}

  /** @brief Конструктор MemoryPool с указанием буфера и его размера
   * @details Создает пул памяти с заданным буфером и его размером.
   *
   * @param buffer Указатель на буфер, который будет использоваться в качестве
   * пула памяти.
   */
  template <size_t N>
  MemoryPool(uint8_t (&buffer)[N])
      : m_buffer(buffer), m_used_bytes(0), m_max_bytes(N), m_buffer_offset(0) {}

  /** @brief Конструктор MemoryPool с указанием буфера и типа. Размер
   * вычисляется автоматически.
   * @details Создает пул памяти с заданным буфером и его размером, который
   * вычисляется на основе типа T и размера массива N.
   *
   * @param buffer Указатель на буфер, который будет использоваться в качестве
   * пула памяти.
   * @tparam T Тип буфера, который должен быть массивом фиксированного размера.
   */
  template <typename T, size_t N>
  MemoryPool(T (&buffer)[N])
      : m_buffer(static_cast<void*>(buffer)),
        m_used_bytes(0),
        m_max_bytes(sizeof(T) * N),
        m_buffer_offset(0) {}

  /** @brief Метод для выделения памяти из пула
   * @details Этот метод выделяет память из пула с учетом выравнивания и
   * возвращает указатель на выделенный блок памяти. Если буфер не задан,
   * используется стандартный оператор new для выделения памяти.
   * @param size Размер блока памяти, который нужно выделить, в байтах.
   * @param align Выравнивание для выделения памяти, в байтах.
   * @param out_alloc_size Указатель на переменную, в которую будет записан
   * размер выделенного блока памяти. Если указатель равен nullptr,
   * то размер не будет возвращен.
   */
  void* allocateRaw(size_t size, size_t align, size_t* out_alloc_size = 0) {
    if (size == 0) return NULL;
    if (m_buffer) {
      uint8_t* base = static_cast<uint8_t*>(m_buffer) + m_buffer_offset;
      size_t space = m_max_bytes - m_buffer_offset;
      size_t misalign = reinterpret_cast<size_t>(base) % align;
      size_t pad = misalign ? (align - misalign) : 0;

      if (size + pad > space) return NULL;

      void* ptr = reinterpret_cast<void*>(base + pad);
      if (out_alloc_size) *out_alloc_size = size + pad;
      m_buffer_offset += size + pad;
      m_used_bytes += size + pad;
      return ptr;
    } else {
      if (m_used_bytes + size > m_max_bytes || size == 0) return NULL;
      void* ptr = reinterpret_cast<void*>(operator new(size));
      if (ptr) {
        if (out_alloc_size) *out_alloc_size = size;
        m_used_bytes += size;
        return ptr;
      }
      return NULL;
    }
  }

  template <typename T>
  void* allocate(size_t count = 1) {
    using Elem = typename ElementType<T>::Type;
    return allocateRaw(sizeof(Elem) * count, AlignmentOf<Elem>::value);
  }
  /** @brief Метод для освобождения памяти в пуле
   * @details Этот метод освобождает память, выделенную из пула, и
   * уменьшает счетчик использованных байт. Если буфер не задан, используется
   * стандартный оператор delete для освобождения памяти.
   * @param ptr Указатель на блок памяти, который нужно освободить.
   * @param size Размер блока памяти, который нужно освободить, в байтах.
   *
   * @note При наличии буфера, метод просто возвращает, так как
   * освобождение памяти не требуется из-за отсутствия индивидуального
   * распределения в режиме арены/буфера.
   *
   * @warning Этот метод не проверяет, был ли указатель ptr ранее выделен
   * из пула памяти. Освобождение памяти, которая не была выделена из пула,
   * может привести к неопределенному поведению.
   */
  void deallocate(void* ptr, size_t size) {
    if (m_buffer != NULL) {
      return;
    }
    if (ptr) {
      operator delete(ptr);
      if (m_used_bytes < size) {
        m_used_bytes = 0;
      } else {
        m_used_bytes -= size;
      }
    }
  }

  /** @brief Получение информации о пуле памяти
   * @details Этот метод возвращает информацию о текущем состоянии пула памяти,
   * включая количество использованных байт и максимальный размер пула.
   * @return Количество использованных байт, максимальный размер пула и текущий
   * смещение в буфере.
   */
  size_t getUsedBytes() const { return m_used_bytes; }

  /** @brief Получение максимального размера пула памяти
   * @details Этот метод возвращает максимальный размер пула памяти,
   * который был задан при создании объекта MemoryPool.
   * @return Максимальный размер пула памяти в байтах.
   */
  size_t getMaxBytes() const { return m_max_bytes; }
  void reset() {
    m_used_bytes = 0;
    m_buffer_offset = 0;
  }

  /** @brief Получение указателя на буфер пула памяти
   * @details Этот метод возвращает указатель на буфер, который используется
   * в качестве пула памяти. Если буфер не задан, возвращается nullptr.
   * @return Указатель на буфер пула памяти или nullptr, если буфер не задан.
   */
  void* getBuffer() const { return m_buffer; }

  /** @brief Получение смещения в буфере, где начинается следующий доступный
   * блок памяти
   * @details Этот метод возвращает смещение в буфере, которое указывает на
   * следующий доступный блок памяти для выделения. Это значение обновляется
   * каждый раз при выделении памяти из пула.
   * @return Смещение в буфере, где начинается следующий доступный блок памяти.
   */
  size_t getBufferOffset() const { return m_buffer_offset; }
};
}  // namespace Knot

#endif  // MEMORY_POOL_HPP
