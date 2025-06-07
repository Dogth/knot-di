#include "../include/MemoryPool.hpp"
#include <cassert>
#include <gtest/gtest.h>

TEST(MemoryPoolTest, AllocateAndDeallocate) {
  Knot::MemoryPool pool(128);
  void *ptr1 = pool.allocate(32, alignof(int));
  ASSERT_NE(ptr1, nullptr);
  EXPECT_GE(pool.getUsedBytes(), 32);

  pool.deallocate(ptr1, 32);
  EXPECT_EQ(pool.getUsedBytes(), 0);
}

TEST(MemoryPoolTest, BufferOverflow) {
  char buffer[64];
  Knot::MemoryPool pool(buffer, sizeof(buffer));
  void *ptr1 = pool.allocate(60, alignof(int));
  ASSERT_NE(ptr1, nullptr);

  void *ptr2 = pool.allocate(8, alignof(int));
  EXPECT_EQ(ptr2, nullptr); // Should fail due to overflow
}

TEST(MemoryPoolTest, Alignment) {
  char buffer[64];
  Knot::MemoryPool pool(buffer, sizeof(buffer));
  void *ptr = pool.allocate(16, 16);
  ASSERT_NE(ptr, nullptr);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % 16, 0);
}

TEST(MemoryPoolTest, ZeroSizeAllocation) {
  Knot::MemoryPool pool(64);
  void *ptr = pool.allocate(0, alignof(int));
  EXPECT_EQ(ptr, nullptr);
  EXPECT_EQ(pool.getUsedBytes(), 0);
}

TEST(MemoryPoolTest, MultipleAllocationsAndDeallocations) {
  Knot::MemoryPool pool(128);
  void *ptr1 = pool.allocate(32, alignof(int));
  void *ptr2 = pool.allocate(32, alignof(int));
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
  void *ptr1 = pool.allocate(32, alignof(int));
  ASSERT_NE(ptr1, nullptr);
  void *ptr2 = pool.allocate(1, alignof(int));
  EXPECT_EQ(ptr2, nullptr);
}

TEST(MemoryPoolTest, ResetFunctionality) {
  char buffer[64];
  Knot::MemoryPool pool(buffer, sizeof(buffer));
  void *ptr1 = pool.allocate(32, alignof(int));
  ASSERT_NE(ptr1, nullptr);
  pool.reset();
  EXPECT_EQ(pool.getUsedBytes(), 0);
  EXPECT_EQ(pool.getBufferOffset(), 0);
  void *ptr2 = pool.allocate(64, alignof(int));
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

  Knot::MemoryPool pool(buffer, sizeof(buffer));
  void *ptr = pool.allocate(32, alignof(int));
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
