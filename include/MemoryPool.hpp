#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include "ContainerMacros.hpp"
#include <cassert>
#include <cstddef>
namespace Knot {
class MemoryPool {
private:
  size_t _used_bytes;
  size_t _max_bytes;
  size_t _buffer_offset;

  void *_buffer;

public:
  MemoryPool(size_t max_bytes)
      : _buffer(nullptr), _used_bytes(0), _max_bytes(max_bytes),
        _buffer_offset(0) {}

  MemoryPool(void *buffer, size_t buffer_size)
      : _buffer(buffer), _used_bytes(0), _max_bytes(buffer_size),
        _buffer_offset(0) {}

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
  void deallocate(void *ptr, size_t size) {
    if (_buffer != NULL) {
      assert(false && "deallocate() not supported in buffer-backed pools");
      return;
    }
    if (ptr) {
      operator delete(ptr);
      if (_used_bytes < size) {
        assert(false && "_used_bytes underflow on deallocate!");
        _used_bytes = 0;
      } else {
        _used_bytes -= size;
      }
    }
  }

  size_t getUsedBytes() const { return _used_bytes; }
  size_t getMaxBytes() const { return _max_bytes; }
  void reset() {
    _used_bytes = 0;
    _buffer_offset = 0;
  }
  void *getBuffer() const { return _buffer; }
  size_t getBufferOffset() const { return _buffer_offset; }
};
} // namespace Knot

#endif // MEMORY_POOL_HPP
