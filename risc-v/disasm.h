#pragma once

#include "risc-v/memory.h"

namespace risc_v {

void Disassemble(Memory* mem, unsigned int addr1, unsigned int addr2);

namespace internal {

void Disassemble(int inst);

}  // namespace internal
}  // namespace risc_v
