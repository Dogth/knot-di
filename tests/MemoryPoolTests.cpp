#include <gtest/gtest.h>

#include <cassert>

#include "../include/knot-di/MemoryPool.hpp"

TEST(MemoryPoolTest, AllocateAndDeallocate) {
  Knot::MemoryPool pool(128);
  void* ptr1 = pool.allocateRaw(32, alignof(int));
  ASSERT_NE(ptr1, nullptr);
  EXPECT_GE(pool.getUsedBytes(), 32);

  pool.deallocate(ptr1, 32);
  EXPECT_EQ(pool.getUsedBytes(), 0);
}

TEST(MemoryPoolTest, AllocateSingleAndArray) {
  char buffer[128];
  Knot::MemoryPool pool(buffer);

  int* intPtr = static_cast<int*>(pool.allocate<int>());
  ASSERT_NE(intPtr, nullptr);
  *intPtr = 42;
  EXPECT_EQ(*intPtr, 42);

  double* dblPtr = static_cast<double*>(pool.allocate<double>(5));
  ASSERT_NE(dblPtr, nullptr);
  for (int i = 0; i < 5; ++i) {
    dblPtr[i] = i * 1.5;
    EXPECT_EQ(dblPtr[i], i * 1.5);
  }
}

TEST(MemoryPoolTest, AllocateAlignment) {
  char buffer[128];
  Knot::MemoryPool pool(buffer);

  struct alignas(32) BigAlign {
    char data[32];
  };
  BigAlign* ptr = static_cast<BigAlign*>(pool.allocate<BigAlign>(2));
  ASSERT_NE(ptr, nullptr);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % alignof(BigAlign), 0);
}

TEST(MemoryPoolTest, AllocateMultipleTypes) {
  char buffer[256];
  Knot::MemoryPool pool(buffer);

  int* a = static_cast<int*>(pool.allocate<int>());
  double* b = static_cast<double*>(pool.allocate<double>());
  char* c = static_cast<char*>(pool.allocate<char>(10));

  ASSERT_NE(a, nullptr);
  ASSERT_NE(b, nullptr);
  ASSERT_NE(c, nullptr);

  *a = 7;
  *b = 3.14;
  strcpy(c, "test");

  EXPECT_EQ(*a, 7);
  EXPECT_EQ(*b, 3.14);
  EXPECT_STREQ(c, "test");
}

TEST(MemoryPoolTest, BufferOverflow) {
  char buffer[64];
  Knot::MemoryPool pool(buffer);
  void* ptr1 = pool.allocateRaw(60, alignof(int));
  ASSERT_NE(ptr1, nullptr);

  void* ptr2 = pool.allocateRaw(8, alignof(int));
  EXPECT_EQ(ptr2, nullptr);  // Should fail due to overflow
}

TEST(MemoryPoolTest, Alignment) {
  char buffer[64];
  Knot::MemoryPool pool(buffer);
  void* ptr = pool.allocateRaw(16, 16);
  ASSERT_NE(ptr, nullptr);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % 16, 0);
}

TEST(MemoryPoolTest, ZeroSizeAllocation) {
  Knot::MemoryPool pool(64);
  void* ptr = pool.allocateRaw(0, alignof(int));
  EXPECT_EQ(ptr, nullptr);
  EXPECT_EQ(pool.getUsedBytes(), 0);
}

TEST(MemoryPoolTest, MultipleAllocationsAndDeallocations) {
  Knot::MemoryPool pool(128);
  void* ptr1 = pool.allocateRaw(32, alignof(int));
  void* ptr2 = pool.allocateRaw(32, alignof(int));
  ASSERT_NE(ptr1, nullptr);
  ASSERT_NE(ptr2, nullptr);
  EXPECT_GE(pool.getUsedBytes(), 64);

  pool.deallocate(ptr2, 32);
  EXPECT_GE(pool.getUsedBytes(), 32);
  pool.deallocate(ptr1, 32);
  EXPECT_EQ(pool.getUsedBytes(), 0);
}

TEST(MemoryPoolTest, HeapOverflow) {
  Knot::MemoryPool pool(32);
  void* ptr1 = pool.allocateRaw(32, alignof(int));
  ASSERT_NE(ptr1, nullptr);
  void* ptr2 = pool.allocateRaw(1, alignof(int));
  EXPECT_EQ(ptr2, nullptr);
}

TEST(MemoryPoolTest, ResetFunctionality) {
  char buffer[64];
  Knot::MemoryPool pool(buffer);
  void* ptr1 = pool.allocateRaw(32, alignof(int));
  ASSERT_NE(ptr1, nullptr);
  pool.reset();
  EXPECT_EQ(pool.getUsedBytes(), 0);
  EXPECT_EQ(pool.getBufferOffset(), 0);
  void* ptr2 = pool.allocateRaw(64, alignof(int));
  ASSERT_NE(ptr2, nullptr);
}

TEST(MemoryPoolTest, MaxBytesQuery) {
  Knot::MemoryPool pool(256);
  EXPECT_EQ(pool.getMaxBytes(), 256);
}

TEST(MemoryPoolTest, BufferDoesNotAffectNearbyMemory) {
  alignas(16) char before[16] = {static_cast<char>(0xAA)};
  alignas(16) char buffer[64];
  alignas(16) char after[16] = {static_cast<char>(0xBB)};

  std::fill_n(before, sizeof(before), 0xAA);
  std::fill_n(after, sizeof(after), 0xBB);

  Knot::MemoryPool pool(buffer);
  void* ptr = pool.allocateRaw(32, alignof(int));
  ASSERT_NE(ptr, nullptr);

  std::memset(ptr, 0xCC, 32);

  for (size_t i = 0; i < sizeof(before); ++i) {
    EXPECT_EQ(before[i], static_cast<char>(0xAA))
        << "Buffer underflow detected at index " << i;
  }
  for (size_t i = 0; i < sizeof(after); ++i) {
    EXPECT_EQ(after[i], static_cast<char>(0xBB))
        << "Buffer overflow detected at index " << i;
  }
}

TEST(MemoryPoolTest, DestructorFreesHeapMemory) {
  size_t max_bytes = 128;
  Knot::MemoryPool* pool = new Knot::MemoryPool(max_bytes);
  void* ptr = pool->allocateRaw(64, alignof(int));
  ASSERT_NE(ptr, nullptr);
  delete pool;  // Should free all memory without leaks or crashes
}
