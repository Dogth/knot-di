#pragma once

#include <cstddef>
#include <new>

template <typename T, std::size_t N> class static_allocator {
public:
  typedef T value_type;
  typedef T *pointer;
  typedef const T *const_pointer;
  typedef T &reference;
  typedef const T &const_reference;
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  template <typename U> struct rebind {
    typedef static_allocator<U, N> other;
  };

  static_allocator() {}
  template <typename U> static_allocator(const static_allocator<U, N> &) {}

  pointer allocate(size_type n, const void * = 0) {
    if (allocated_ + n > N)
      throw std::bad_alloc();
    pointer p = reinterpret_cast<pointer>(&buffer_[allocated_]);
    allocated_ += n;
    return p;
  }

  void deallocate(pointer, size_type) {
    // No-op: memory is never freed
  }

  size_type max_size() const { return N; }

private:
  static char buffer_[sizeof(T) * N];
  static size_type allocated_;
};

template <typename T, std::size_t N>
char static_allocator<T, N>::buffer_[sizeof(T) * N];

template <typename T, std::size_t N>
std::size_t static_allocator<T, N>::allocated_ = 0;
