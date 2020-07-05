#include "risc-v/memory.h"

#include <gtest/gtest.h>

namespace risc_v {

TEST(MemoryTest, Word) {
    Memory mem;
    EXPECT_EQ(mem.GetWord(0), 0);
    mem.PutWord(0, 1);
    EXPECT_EQ(mem.GetWord(0), 1);
}

TEST(MemoryTest, Mem) {
    {
        Memory mem;
        EXPECT_EQ(mem.GetMem(0, Memory::Size::kWord), 0);
        mem.PutMem(0, 1, Memory::Size::kWord);
        EXPECT_EQ(mem.GetMem(0, Memory::Size::kWord), 1);
    }
    {
        Memory mem;
        EXPECT_EQ(mem.GetMem(0, Memory::Size::kHWord), 0);
        EXPECT_EQ(mem.GetMem(2, Memory::Size::kHWord), 0);
        mem.PutMem(0, 1, Memory::Size::kHWord);
        EXPECT_EQ(mem.GetMem(0, Memory::Size::kHWord), 1);
        EXPECT_EQ(mem.GetMem(2, Memory::Size::kHWord), 0);
        mem.PutMem(2, 1, Memory::Size::kHWord);
        EXPECT_EQ(mem.GetMem(0, Memory::Size::kHWord), 1);
        EXPECT_EQ(mem.GetMem(2, Memory::Size::kHWord), 1);
    }
    {
        Memory mem;
        EXPECT_EQ(mem.GetMem(0, Memory::Size::kByte), 0);
        EXPECT_EQ(mem.GetMem(1, Memory::Size::kByte), 0);
        EXPECT_EQ(mem.GetMem(2, Memory::Size::kByte), 0);
        EXPECT_EQ(mem.GetMem(3, Memory::Size::kByte), 0);
        mem.PutMem(0, 1, Memory::Size::kByte);        
        EXPECT_EQ(mem.GetMem(0, Memory::Size::kByte), 1);
        EXPECT_EQ(mem.GetMem(1, Memory::Size::kByte), 0);
        EXPECT_EQ(mem.GetMem(2, Memory::Size::kByte), 0);
        EXPECT_EQ(mem.GetMem(3, Memory::Size::kByte), 0);
        mem.PutMem(1, 1, Memory::Size::kByte);        
        EXPECT_EQ(mem.GetMem(0, Memory::Size::kByte), 1);
        EXPECT_EQ(mem.GetMem(1, Memory::Size::kByte), 1);
        EXPECT_EQ(mem.GetMem(2, Memory::Size::kByte), 0);
        EXPECT_EQ(mem.GetMem(3, Memory::Size::kByte), 0);
        mem.PutMem(2, 1, Memory::Size::kByte);        
        EXPECT_EQ(mem.GetMem(0, Memory::Size::kByte), 1);
        EXPECT_EQ(mem.GetMem(1, Memory::Size::kByte), 1);
        EXPECT_EQ(mem.GetMem(2, Memory::Size::kByte), 1);
        EXPECT_EQ(mem.GetMem(3, Memory::Size::kByte), 0);
        mem.PutMem(3, 1, Memory::Size::kByte);        
        EXPECT_EQ(mem.GetMem(0, Memory::Size::kByte), 1);
        EXPECT_EQ(mem.GetMem(1, Memory::Size::kByte), 1);
        EXPECT_EQ(mem.GetMem(2, Memory::Size::kByte), 1);
        EXPECT_EQ(mem.GetMem(3, Memory::Size::kByte), 1);
    }
}

TEST(MemoryTest, Search) {
    Memory mem;
    for (auto i = 0; i < kMaxPageNum; ++i) {
        int addr = i << 16;
        mem.PutWord(addr, 1);
        EXPECT_EQ(mem.GetWord(addr), 1);
    }
    mem.PutWord(kMaxPageNum << 16, 1);  // out of memory
    EXPECT_NE(mem.GetWord(kMaxPageNum << 16), 1);
}

}  // namespace risc_v
