#include "risc-v/disasm.h"

#include <gtest/gtest.h>

namespace risc_v {

TEST(DisasmTest, LUI) {
    testing::internal::CaptureStdout();
    internal::Disassemble(0b0000'0000'0000'0000'0001'00001'0110111);
    std::string out = testing::internal::GetCapturedStdout();
    EXPECT_EQ(out, "lui\tx1,1\n");
}

TEST(DisasmTest, AUIPC) {
    testing::internal::CaptureStdout();
    internal::Disassemble(0b0000'0000'0000'0000'0001'00001'0010111);
    std::string out = testing::internal::GetCapturedStdout();
    EXPECT_EQ(out, "auipc\tx1,1\n");
}

TEST(DisasmTest, JAL) {
    testing::internal::CaptureStdout();
    internal::Disassemble(0b0'0000000001'1'00000001'00001'1101111);
    std::string out = testing::internal::GetCapturedStdout();
    EXPECT_EQ(out, "jal\tx1,6146\n");
}

}  // namespace risc_v
