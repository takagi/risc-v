#pragma once

#include "risc-v/memory.h"
#include "risc-v/reg_file.h"

namespace risc_v {
namespace internal {

struct Machine {
    RegFile rf;
    Memory mem;

    int inst_reg  = 0;
    int opcode    = 0;
    int rd        = 0;
    int funct3    = 0;
    int rs1       = 0;
    int rs2       = 0;
    int funct7    = 0;
    int i_imm_zex = 0;
    int i_imm_sex = 0;
    int s_imm_zex = 0;
    int s_imm_sex = 0;
    int b_imm_zex = 0;
    int b_imm_sex = 0;
    int u_imm_zex = 0;
    int u_imm_sex = 0;
    int j_imm_zex = 0;
    int j_imm_sex = 0;

    void Fetch();
    void Decode();
    void Execute();
};

}  // namespace internal

class Machine {
public:
    void SetPC(int value);
    void DumpReg() const;

    void LoadElf(const char* fname);

    void Trace();

private:
    internal::Machine m_;
};

}  // namespace risc_v
