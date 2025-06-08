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
namespace Knot {
/** @brief Класс MemoryPool для управления памятью
 *
 * Этот класс предоставляет функциональность для управления памятью с
 * использованием пула памяти. Он позволяет выделять и освобождать память,
 * а также управлять размером пула.
 */
class MemoryPool {
private:
  size_t _used_bytes;    // Количество использованных байт в пуле памяти
  size_t _max_bytes;     // Максимальный размер пула памяти
  size_t _buffer_offset; // Смещение в буфере, где начинается следующий
                         // доступный блок памяти

  void *_buffer; // Указатель на буфер, используемый в качестве пула памяти

public:
  /** @brief Конструктор MemoryPool без параметров
   * @details Создает пул памяти с максимальным размером,
   * заданным в параметре max_bytes.
   *
   * @param max_bytes Максимальный размер пула памяти в байтах.
   */
  MemoryPool(size_t max_bytes)
      : _buffer(nullptr), _used_bytes(0), _max_bytes(max_bytes),
        _buffer_offset(0) {}

  /** @brief Конструктор MemoryPool с указанием буфера и его размера
   * @details Создает пул памяти с заданным буфером и его размером.
   *
   * @param buffer Указатель на буфер, который будет использоваться в качестве
   * пула памяти.
   * @param buffer_size Размер буфера в байтах.
   */
  MemoryPool(void *buffer, size_t buffer_size)
      : _buffer(buffer), _used_bytes(0), _max_bytes(buffer_size),
        _buffer_offset(0) {}

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
  void *allocate(size_t size, size_t align, size_t *out_alloc_size = 0) {
    if (size == 0)
      return nullptr;
    if (_buffer) {
      char *base = static_cast<char *>(_buffer) + _buffer_offset;
      size_t space = _max_bytes - _buffer_offset;
      size_t misalign = reinterpret_cast<size_t>(base) % align;
      size_t pad = misalign ? (align - misalign) : 0;

      if (size + pad > space)
        return nullptr;

      void *ptr = base + pad;
      if (out_alloc_size)
        *out_alloc_size = size + pad;
      _buffer_offset += size + pad;
      _used_bytes += size + pad;
      return ptr;
    } else {
      if (_used_bytes + size > _max_bytes || size == 0)
        return nullptr;
      void *ptr = operator new(size);
      if (ptr) {
        if (out_alloc_size)
          *out_alloc_size = size;
        _used_bytes += size;
        return ptr;
      }
      return nullptr;
    }
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
  void deallocate(void *ptr, size_t size) {
    if (_buffer != NULL) {
      return;
    }
    if (ptr) {
      operator delete(ptr);
      if (_used_bytes < size) {
        _used_bytes = 0;
      } else {
        _used_bytes -= size;
      }
    }
  }

  /** @brief Получение информации о пуле памяти
   * @details Этот метод возвращает информацию о текущем состоянии пула памяти,
   * включая количество использованных байт и максимальный размер пула.
   * @return Количество использованных байт, максимальный размер пула и текущий
   * смещение в буфере.
   */
  size_t getUsedBytes() const { return _used_bytes; }

  /** @brief Получение максимального размера пула памяти
   * @details Этот метод возвращает максимальный размер пула памяти,
   * который был задан при создании объекта MemoryPool.
   * @return Максимальный размер пула памяти в байтах.
   */
  size_t getMaxBytes() const { return _max_bytes; }
  void reset() {
    _used_bytes = 0;
    _buffer_offset = 0;
  }

  /** @brief Получение указателя на буфер пула памяти
   * @details Этот метод возвращает указатель на буфер, который используется
   * в качестве пула памяти. Если буфер не задан, возвращается nullptr.
   * @return Указатель на буфер пула памяти или nullptr, если буфер не задан.
   */
  void *getBuffer() const { return _buffer; }

  /** @brief Получение смещения в буфере, где начинается следующий доступный
   * блок памяти
   * @details Этот метод возвращает смещение в буфере, которое указывает на
   * следующий доступный блок памяти для выделения. Это значение обновляется
   * каждый раз при выделении памяти из пула.
   * @return Смещение в буфере, где начинается следующий доступный блок памяти.
   */
  size_t getBufferOffset() const { return _buffer_offset; }
};
} // namespace Knot

#endif // MEMORY_POOL_HPP
