#include "risc-v/disasm.h"

#include <cassert>
#include <cstdio>

namespace risc_v {

void Disassemble(Memory* mem, unsigned int addr1, unsigned int addr2) {
    for (auto i = addr1; i <= addr2; i+= 4) {
        printf("%08x ", i);
        internal::Disassemble(mem->GetWord(i));
    }
}

namespace internal {

void Disassemble(int inst) {
    int opcode = inst & 0x7f;
    int rd = (inst >> 7) & 0x1f;
    int funct3 = (inst >> 12) & 0x07;
    int rs1 = (inst >> 15) & 0x1f;
    int rs2 = (inst >> 20) & 0x1f;
    int funct7 = (inst >> 25) & 0x3f;
    int i_imm_zex = (inst >> 20) & 0xfff;
    int i_imm_sex = (i_imm_zex & (1 << 11)) ? 0xfffff000 | i_imm_zex
                                            : i_imm_zex;
    int s_imm_zex = (((inst >> 25) & 0x7f) << 5) | ((inst >> 7) & 0x1f);
    int s_imm_sex = (s_imm_zex & 0x800) ? (0xfffff000 | s_imm_zex)
                                        : s_imm_zex;
    int b_imm_zex = (((inst >> 31) & 0x01) << 12)
                  | (((inst >>  7) & 0x01) << 11)
                  | (((inst >> 25) & 0x3f) << 5)
                  | (((inst >>  8) & 0x0f) << 1);
    int b_imm_sex = (b_imm_zex & 0x1000) ? 0xffffe000 | b_imm_zex
                                         : b_imm_zex;
    int u_imm_zex = inst & 0xffff'f000;
    int u_imm_sex = u_imm_zex;
    int j_imm_zex = (((inst >> 31) & 0x01) << 20)
                  | (((inst >> 12) & 0xff) << 12)
                  | (((inst >> 20) & 0x01) << 11)
                  | (((inst >> 21) & 0x3ff) << 1);
    int j_imm_sex = (j_imm_zex & (1 << 20)) ? 0xffe00000 | j_imm_zex
                                            : j_imm_zex;

    switch (opcode) {
    case 0x37:  // LUI
        printf("lui\tx%d,%d\n", rd, u_imm_sex >> 12);
        return;
    case 0x17:  // AUIPC
        printf("auipc\tx%d,%d\n", rd, u_imm_sex >> 12);
        return;
    case 0x6f:  // JAL
        printf("jal\tx%d,%d\n", rd, j_imm_zex);
        return;
    default:
        assert(false);
    }
}

}  // namespace internal
}  // namespace risc_v
