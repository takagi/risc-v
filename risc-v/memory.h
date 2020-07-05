#pragma once

#include "risc-v/constant.h"

namespace risc_v {

class Memory {
public:
    enum class Size {
        kWord,
        kHWord,
        kByte,
    };

    using Unit = Size;

    Memory();

    int GetWord(int addr);
    void PutWord(int addr, int data);

    int GetMem(int addr, Size size);
    void PutMem(int addr, int value, Size size);

    void Dump(unsigned int start, unsigned int end, Unit unit);

private:
    int Search(int addr);

    char mem_[kMaxPageNum][0x10000];
    int pair_ptr_;
    int pair_[kMaxPageNum];
};

}  // namespace risc_v
