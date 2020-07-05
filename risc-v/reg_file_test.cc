#include "risc-v/reg_file.h"

#include <gtest/gtest.h>

namespace risc_v {

TEST(RegFileTest, Ctor) {
    RegFile rf;
    EXPECT_EQ(rf.GetPC(), 0);
    for(auto i = 0; i < 32; ++i) {
        EXPECT_EQ(rf.GetReg(i), 0);
    }
}

TEST(RegFileTest, PC) {
    RegFile rf;
    EXPECT_EQ(rf.GetPC(), 0);
    rf.PutPC(1);
    EXPECT_EQ(rf.GetPC(), 1);
    rf.IncPC(2);
    EXPECT_EQ(rf.GetPC(), 3);
}

TEST(RegFileTest, Reg) {
    {
        RegFile rf;
        EXPECT_EQ(rf.GetReg(1), 0);
        rf.PutReg(1, 1);
        EXPECT_EQ(rf.GetReg(1), 1);
    }
    {
        RegFile rf;
        EXPECT_EQ(rf.GetReg(0), 0);
        rf.PutReg(0, 1);
        EXPECT_EQ(rf.GetReg(0), 0);
    }
}

TEST(RegFileTest, Dump) {
    RegFile rf;
    rf.Dump();
}

}  // namespace risc_v
