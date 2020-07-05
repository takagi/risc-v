#include "risc-v/machine.h"

#include <cassert>
#include <cstdio>
#include <initializer_list>

#include "risc-v/load.h"

namespace risc_v {
namespace {

bool (* const kBranchFunct3[])(const int&, const int&) = {
    [](const int& src1, const int& src2) { return src1 == src2; },  // BEQ
    [](const int& src1, const int& src2) { return src1 != src2; },  // BNE
    nullptr,  // unused
    nullptr,  // unused
    [](const int& src1, const int& src2) { return src1 < src2; },   // BLT
    [](const int& src1, const int& src2) { return src1 >= src2; },  // BGE
    [](const int& src1, const int& src2) {
        return (unsigned int)src1 < (unsigned int)src2; },        // BLTU
    [](const int& src1, const int& src2) {
        return (unsigned int)src1 >= (unsigned int)src2; },       // BGEU
};

template<class T>
T identity(T t) { return t; }

struct LoadFunct3Param {
    Memory::Size size;
    int (*extension)(int word);
};

const LoadFunct3Param kLoadFunct3Params[] = {
    // LB
    { Memory::Size::kByte,
      [](int word) -> int {
          return (word & 0x80) ? (word | 0xffffff00) : (word & 0x000000ff);
      }
    },
    // LH
    { Memory::Size::kHWord,
      [](int word) -> int {
          return (word & 0x8000) ? (word | 0xffff0000) : (word & 0x0000ffff);
      }
    },
    // LW
    { Memory::Size::kWord, identity },
    // unused
    { Memory::Size::kByte, nullptr },
    // LBU
    { Memory::Size::kByte, identity },
    // LHU
    { Memory::Size::kHWord, identity },
};

const Memory::Size kStoreFunct3[] = {
    Memory::Size::kByte,   // SB
    Memory::Size::kHWord,  // SH
    Memory::Size::kWord,   // SW
};

int (*const kOpImmFunct3[])(int, int) = {
    [](int src, int imm) { return src + imm; },  // ADDI
    [](int src, int imm) { return src << (imm & 0x1f); },  // SLLI
    [](int src, int imm) { return (src < imm) ? 1 : 0; },  // LSTI
    [](int src, int imm) {  // LSTIU
        auto usrc = static_cast<unsigned int>(src);
        auto uimm = static_cast<unsigned int>(imm);
        return (usrc < uimm) ? 1 : 0;
    },
    [](int src, int imm) { return src ^ imm; },  // XORI
    [](int src, int imm) -> int {  // SRLI/SRAI
        if (imm >> 5) {
            return src >> (imm & 0x1f);
        }
        auto usrc = static_cast<unsigned int>(src);
        auto uimm = static_cast<unsigned int>(imm);
        return usrc >> uimm;
    },
    [](int src, int imm) { return src | imm; },  // ORI
    [](int src, int imm) { return src & imm; },  // ANDI
};

int (*const kOpFunct3[])(int, int, int) = {
    [](int src1, int src2, int funct7) {  // ADD/SUB
        if (funct7) {
            return src1 - src2;
        }
        return src1 + src2;
    },
    [](int src1, int src2, int) { return src1 << (src2 & 0x1f); },  // SLL
    [](int src1, int src2, int) { return (src1 < src2) ? 1 : 0; },  // SLT
    [](int src1, int src2, int) {  // SLTU
        auto usrc1 = static_cast<unsigned int>(src1);
        auto usrc2 = static_cast<unsigned int>(src2);
        return (usrc1 < usrc2) ? 1 : 0;
    },
    [](int src1, int src2, int) { return src1 ^ src2; },  // XOR
    [](int src1, int src2, int fucnt7) -> int {  // SRL/SRA
        if (fucnt7) {
            return src1 >> (src2 & 0x1f);
        }
        auto usrc1 = static_cast<unsigned int>(src1);
        auto usrc2 = static_cast<unsigned int>(src2);
        return usrc1 >> (usrc2 & 0x1f);
    },
    [](int src1, int src2, int) { return src1 | src2; },  // OR
    [](int src1, int src2, int) { return src1 & src2; }, // AND
};

}  // namespace

void internal::Machine::Fetch() {
    int pc = rf.GetPC();
    inst_reg = mem.GetWord(pc);
}

void internal::Machine::Decode() {
    opcode = inst_reg & 0x7f;
    rd = (inst_reg >> 7) & 0x1f;
    funct3 = (inst_reg >> 12) & 0x07;
    rs1 = (inst_reg >> 15) & 0x1f;
    rs2 = (inst_reg >> 20) & 0x1f;
    funct7 = (inst_reg >> 25) & 0x3f;
    i_imm_zex = (inst_reg >> 20) & 0xfff;
    i_imm_sex = (i_imm_zex & (1 << 11)) ? 0xfffff000 | i_imm_zex
                                        : i_imm_zex;
    s_imm_zex = (((inst_reg >> 25) & 0x7f) << 5) | ((inst_reg >> 7) & 0x1f);
    s_imm_sex = (s_imm_zex & 0x800) ? (0xfffff000 | s_imm_zex)
                                    : s_imm_zex;
    b_imm_zex = (((inst_reg >> 31) & 0x01) << 12)
              | (((inst_reg >>  7) & 0x01) << 11)
              | (((inst_reg >> 25) & 0x3f) << 5)
              | (((inst_reg >>  8) & 0x0f) << 1);
    b_imm_sex = (b_imm_zex & 0x1000) ? 0xffffe000 | b_imm_zex
                                     : b_imm_zex;
    u_imm_zex = inst_reg & 0xffff'f000;
    u_imm_sex = u_imm_zex;
    j_imm_zex = (((inst_reg >> 31) & 0x01) << 20)
              | (((inst_reg >> 12) & 0xff) << 12)
              | (((inst_reg >> 20) & 0x01) << 11)
              | (((inst_reg >> 21) & 0x3ff) << 1);
    j_imm_sex = (j_imm_zex & (1 << 20)) ? 0xffe00000 | j_imm_zex
                                        : j_imm_zex;
}

void internal::Machine::Execute() {
    int word;

    switch (opcode) {
    case 0x37:  // LUI
        rf.PutReg(rd, u_imm_sex);
        rf.IncPC(4);
       return;
    case 0x17:  // AUIPC
        rf.PutReg(rd, u_imm_sex + rf.GetPC());
        rf.IncPC(4);
        return;
    case 0x6f:  // JAL
        rf.PutReg(rd, rf.GetPC() + 4);
        rf.IncPC(j_imm_sex);
        return;
    case 0x67:  // JALR
        rf.PutReg(rd, rf.GetPC() + 4);
        rf.IncPC(rf.GetReg(rs1) + i_imm_sex);
        return;
    case 0x63:  // BRANCH
        if (kBranchFunct3[funct3](rf.GetReg(rs1), rf.GetReg(rs2))) {
            rf.IncPC(b_imm_sex);
            return;
        }
        rf.IncPC(4);
        return;
    case 0x03:  // LOAD
    {
        LoadFunct3Param param = kLoadFunct3Params[funct3];
        word = mem.GetMem(rf.GetReg(rs1) + i_imm_sex, param.size);
        word = param.extension(word);
        rf.PutReg(rd, word);
        rf.IncPC(4);
        return;
    }
    case 0x23:  // STORE
    {
        Memory::Size size = kStoreFunct3[funct3];
        word = rf.GetReg(rs2);
        mem.PutMem(rf.GetReg(rs1) + s_imm_sex, word, size);
        rf.IncPC(4);
        return;
    }
    case 0x13:  // OP-IMM
        rf.PutReg(rd, kOpImmFunct3[funct3](rf.GetReg(rs1), i_imm_sex));
        rf.IncPC(4);
        return;
    case 0x33:  // OP
        rf.PutReg(
            rd, kOpFunct3[funct3](rf.GetReg(rs1), rf.GetReg(rs2), funct7));
        rf.IncPC(4);
        return;
    case 0x0f:  // FENCE
    case 0x73:  // ECALL/EBREAK
    default:
        assert(false);
    }
}

void Machine::DumpReg() const {
    m_.rf.Dump();
}

void Machine::SetPC(int value) {
    m_.rf.PutPC(value);
}

void Machine::LoadElf(const char* fname) {
    LoadE(&m_.mem, fname);
}

void Machine::Trace() {
    m_.Fetch();
    m_.Decode();
    m_.Execute();
}

}  // namespace risc_v
