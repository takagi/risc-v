#include "risc-v/machine.h"

#include <gtest/gtest.h>

namespace risc_v {

TEST(MachineTest, Fetch) {
    internal::Machine m;
    EXPECT_EQ(m.inst_reg, 0);
    m.rf.PutPC(0);
    m.mem.PutWord(0, 1);
    m.Fetch();
    EXPECT_EQ(m.inst_reg, 1);
}

TEST(MachineTest, Decode) {
}

TEST(MachineTest, LUI) {
    {
        internal::Machine m;
        m.inst_reg = 0b0000'0000'0000'0000'0001'00001'0110111;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(1), 0x0000'1000);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
    {
        internal::Machine m;
        m.inst_reg = 0b1000'0000'0000'0000'0000'00001'0110111;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(1), 0x8000'0000);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
}

TEST(MachineTest, AUIPC) {
    internal::Machine m;
    m.rf.PutPC(4);
    m.inst_reg = 0b0000'0000'0000'0000'0001'00001'0010111;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(1), 0x0000'1004);
    EXPECT_EQ(m.rf.GetPC(), 8);
}

TEST(MachineTest, JAL) {
    {
        internal::Machine m;
        m.rf.PutPC(4);
        m.inst_reg = 0b0'0000000001'1'00000001'00001'1101111;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(1), 8);
        EXPECT_EQ(m.rf.GetPC(), 0x0000'1806);
    }
    {
        internal::Machine m;
        m.rf.PutPC(4);
        m.inst_reg = 0b1'1111111110'1'11111111'00001'1101111;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(1), 8);
        EXPECT_EQ(m.rf.GetPC(), 0x0000'0000);
    }
}

TEST(MachineTest, JALR) {
    internal::Machine m;
    m.rf.PutPC(4);
    m.rf.PutReg(2, 4);
    m.inst_reg = 0b000000001000'00010'000'00001'1100111;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(1), 8);
    EXPECT_EQ(m.rf.GetPC(), 0x0000'0010);
}

TEST(MachineTest, BEQ) {
    {
        internal::Machine m;
        m.rf.PutPC(4);
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, 1);
        m.inst_reg = 0b0'000000'00010'00001'000'0100'0'1100011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetPC(), 12);
    }
    {
        internal::Machine m;
        m.rf.PutPC(4);
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, 2);
        m.inst_reg = 0b0'000000'00010'00001'000'0100'0'1100011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetPC(), 8);
    }
}

TEST(MachineTest, BNE) {
    {
        internal::Machine m;
        m.rf.PutPC(4);
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, 2);
        m.inst_reg = 0b0'000000'00010'00001'001'0100'0'1100011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetPC(), 12);
    }
    {
        internal::Machine m;
        m.rf.PutPC(4);
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, 1);
        m.inst_reg = 0b0'000000'00010'00001'001'0100'0'1100011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetPC(), 8);
    }
}

TEST(MachineTest, BLT) {
    {
        internal::Machine m;
        m.rf.PutPC(4);
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, 2);
        m.inst_reg = 0b0'000000'00010'00001'100'0100'0'1100011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetPC(), 12);
    }
    {
        internal::Machine m;
        m.rf.PutPC(4);
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, 1);
        m.inst_reg = 0b0'000000'00010'00001'100'0100'0'1100011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetPC(), 8);
    }
}

TEST(MachineTest, BGE) {
    {
        internal::Machine m;
        m.rf.PutPC(4);
        m.rf.PutReg(1, 2);
        m.rf.PutReg(2, 1);
        m.inst_reg = 0b0'000000'00010'00001'101'0100'0'1100011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetPC(), 12);
    }
    {
        internal::Machine m;
        m.rf.PutPC(4);
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, 1);
        m.inst_reg = 0b0'000000'00010'00001'101'0100'0'1100011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetPC(), 12);
    }
    {
        internal::Machine m;
        m.rf.PutPC(4);
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, 2);
        m.inst_reg = 0b0'000000'00010'00001'101'0100'0'1100011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetPC(), 8);
    }
}

TEST(MachineTest, BLTU) {
    internal::Machine m;
    m.rf.PutPC(4);
    m.rf.PutReg(1, 1);
    m.rf.PutReg(2, -1);
    m.inst_reg = 0b0'000000'00010'00001'110'0100'0'1100011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetPC(), 12);
}

TEST(MachineTest, BGEU) {
    internal::Machine m;
    m.rf.PutPC(4);
    m.rf.PutReg(1, -1);
    m.rf.PutReg(2, 1);
    m.inst_reg = 0b0'000000'00010'00001'111'0100'0'1100011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetPC(), 12);
}

TEST(MachineTest, LB) {
    {
        internal::Machine m;
        m.rf.PutReg(1, 4);
        m.mem.PutWord(8, 0x0101);
        m.inst_reg = 0b000000000100'00001'000'00010'0000011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 1);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, 4);
        m.mem.PutWord(4, 0xff);
        m.inst_reg = 0b000000000000'00001'000'00010'0000011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), -1);
    }
}

TEST(MachineTest, LH) {
    {
        internal::Machine m;
        m.rf.PutReg(1, 4);
        m.mem.PutWord(8, 0x0001'0001);
        m.inst_reg = 0b000000000100'00001'001'00010'0000011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 1);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, 4);
        m.mem.PutWord(4, 0xffff);
        m.inst_reg = 0b000000000000'00001'001'00010'0000011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), -1);
    }
}

TEST(MachineTest, LW) {
    {
        internal::Machine m;
        m.rf.PutReg(1, 4);
        m.mem.PutWord(8, 0x0000'0001);
        m.inst_reg = 0b000000000100'00001'010'00010'0000011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 1);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, 4);
        m.mem.PutWord(4, 0xffffffff);
        m.inst_reg = 0b000000000000'00001'010'00010'0000011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), -1);
    }
}

TEST(MachineTest, LBU) {
    {
        internal::Machine m;
        m.rf.PutReg(1, 4);
        m.mem.PutWord(8, 0x0101);
        m.inst_reg = 0b000000000100'00001'100'00010'0000011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 1);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, 4);
        m.mem.PutWord(4, 0xff);
        m.inst_reg = 0b000000000000'00001'100'00010'0000011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 255);
    }
}

TEST(MachineTest, LHU) {
    {
        internal::Machine m;
        m.rf.PutReg(1, 4);
        m.mem.PutWord(8, 0x0001'0001);
        m.inst_reg = 0b000000000100'00001'101'00010'0000011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 1);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, 4);
        m.mem.PutWord(4, 0xffff);
        m.inst_reg = 0b000000000000'00001'101'00010'0000011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 65535);
    }
}

TEST(MachineTest, SB) {
    internal::Machine m;
    m.rf.PutReg(1, 1);       // rs1
    m.rf.PutReg(2, 0x0101);  // rs2
    m.inst_reg = 0b0000000'00010'00001'000'00010'0100011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.mem.GetMem(3, Memory::Size::kByte), 0x01);
}

TEST(MachineTest, SH) {
    internal::Machine m;
    m.rf.PutReg(1, 1);         // rs1
    m.rf.PutReg(2, 0x011001);  // rs2
    m.inst_reg = 0b0000000'00010'00001'001'00011'0100011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.mem.GetMem(4, Memory::Size::kHWord), 0x1001);
    EXPECT_EQ(m.mem.GetMem(6, Memory::Size::kHWord), 0x0);
}

TEST(MachineTest, SW) {
    internal::Machine m;
    m.rf.PutReg(1, 1);            // rs1
    m.rf.PutReg(2, 0x0001'0001);  // rs2
    m.inst_reg = 0b0000000'00010'00001'010'00011'0100011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.mem.GetMem(4, Memory::Size::kWord), 0x0001'0001);
}

TEST(MachineTest, ADDI) {
    {
        internal::Machine m;
        m.rf.PutReg(1, 1);
        m.inst_reg = 0b000000000010'00001'000'00010'0010011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 3);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, 0xffff'ffff);
        m.inst_reg = 0b000000000010'00001'000'00010'0010011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 1);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, 0);
        m.inst_reg = 0b111111111111'00001'000'00010'0010011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), -1);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
}

TEST(MachineTest, SLTI) {
    {
        internal::Machine m;
        m.rf.PutReg(1, 0);  // rs1
        m.inst_reg = 0b000000000001'00001'010'00010'0010011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 1);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, 1);  // rs1
        m.inst_reg = 0b000000000000'00001'010'00010'0010011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 0);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, 0);  // rs1
        m.inst_reg = 0b000000000000'00001'010'00010'0010011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 0);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, -2);  // rs1
        m.inst_reg = 0b111111111111'00001'010'00010'0010011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 1);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
}

TEST(MachineTest, SLTIU) {
    {
        internal::Machine m;
        m.rf.PutReg(1, 1);  // rs1
        m.inst_reg = 0b111111111111'00001'011'00010'0010011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(2), 1);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
}

TEST(MachineTest, XORI) {
    internal::Machine m;
    m.rf.PutReg(1, 0b1010);  // rs1
    m.inst_reg = 0b000000001001'00001'100'00010'0010011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(2), 0b0011);
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, ORI) {
    internal::Machine m;
    m.rf.PutReg(1, 0b1010);  // rs1
    m.inst_reg = 0b000000001001'00001'110'00010'0010011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(2), 0b1011);
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, ANDI) {
    internal::Machine m;
    m.rf.PutReg(1, 0b1010);  // rs1
    m.inst_reg = 0b000000001001'00001'111'00010'0010011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(2), 0b1000);
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, SLLI) {
    internal::Machine m;
    m.rf.PutReg(1, 0b1);  // rs1
    m.inst_reg = 0b000000000010'00001'001'00010'0010011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(2), 0b100);
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, SRLI) {
    internal::Machine m;
    m.rf.PutReg(1, static_cast<int>(0x8000'0002));  // rs1
    m.inst_reg = 0b000000000001'00001'101'00010'0010011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(2), 0x4000'0001);
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, SRAI) {
    internal::Machine m;
    m.rf.PutReg(1, static_cast<int>(0x8000'0002));  // rs1
    m.inst_reg = 0b010000000001'00001'101'00010'0010011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(2), static_cast<int>(0xc000'0001));
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, ADD) {
    internal::Machine m;
    m.rf.PutReg(1, 1);
    m.rf.PutReg(2, 2);
    m.inst_reg = 0b0000000'00010'00001'000'00011'0110011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(3), 3);
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, SUB) {
    {
        internal::Machine m;
        m.rf.PutReg(1, 2);
        m.rf.PutReg(2, 1);
        m.inst_reg = 0b0100000'00010'00001'000'00011'0110011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(3), 1);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, 2);
        m.inst_reg = 0b0100000'00010'00001'000'00011'0110011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(3), -1);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
}

TEST(MachineTest, SLL) {
    internal::Machine m;
    m.rf.PutReg(1, 1);
    m.rf.PutReg(2, 2);
    m.inst_reg = 0b0000000'00010'00001'001'00011'0110011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(3), 4);
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, SLT) {
    {
        internal::Machine m;
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, 2);
        m.inst_reg = 0b0000000'00010'00001'010'00011'0110011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(3), 1);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, 1);
        m.inst_reg = 0b0000000'00010'00001'010'00011'0110011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(3), 0);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
}

TEST(MachineTest, SLTU) {
    {
        internal::Machine m;
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, 2);
        m.inst_reg = 0b0000000'00010'00001'011'00011'0110011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(3), 1);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
    {
        internal::Machine m;
        m.rf.PutReg(1, 1);
        m.rf.PutReg(2, -1);
        m.inst_reg = 0b0000000'00010'00001'011'00011'0110011;
        m.Decode();
        m.Execute();
        EXPECT_EQ(m.rf.GetReg(3), 1);
        EXPECT_EQ(m.rf.GetPC(), 4);
    }
}

TEST(MachineTest, XOR) {
    internal::Machine m;
    m.rf.PutReg(1, 0b1010);
    m.rf.PutReg(2, 0b1001);
    m.inst_reg = 0b0000000'00010'00001'100'00011'0110011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(3), 0b0011);
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, SRL) {
    internal::Machine m;
    m.rf.PutReg(1, static_cast<int>(0x8000'0002));
    m.rf.PutReg(2, 0x1);
    m.inst_reg = 0b0000000'00010'00001'101'00011'0110011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(3), 0x4000'0001);
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, SRA) {
    internal::Machine m;
    m.rf.PutReg(1, static_cast<int>(0x8000'0002));
    m.rf.PutReg(2, 0x1);
    m.inst_reg = 0b0100000'00010'00001'101'00011'0110011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(3), static_cast<int>(0xc000'0001));
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, OR) {
    internal::Machine m;
    m.rf.PutReg(1, 0b1010);
    m.rf.PutReg(2, 0b1001);
    m.inst_reg = 0b0000000'00010'00001'110'00011'0110011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(3), 0b1011);
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, AND) {
    internal::Machine m;
    m.rf.PutReg(1, 0b1010);
    m.rf.PutReg(2, 0b1001);
    m.inst_reg = 0b0000000'00010'00001'111'00011'0110011;
    m.Decode();
    m.Execute();
    EXPECT_EQ(m.rf.GetReg(3), 0b1000);
    EXPECT_EQ(m.rf.GetPC(), 4);
}

TEST(MachineTest, FENCE) {
    // Not implemented.
}


TEST(MachineTest, ECALL) {
    // Not implemented.
}

TEST(MachineTest, EBREAK) {
    // Not implemented.
}

}  // namespace risc_v
