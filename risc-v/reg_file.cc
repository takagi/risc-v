#include "risc-v/reg_file.h"

#include <cstdio>
#include <cstring>

namespace risc_v {

RegFile::RegFile(): pc_(0) {
    memset(rf_, 0, 32 * sizeof(int));
}

int RegFile::GetPC() const {
    return pc_;
}

void RegFile::PutPC(int value) {
    pc_ = value;
}

void RegFile::IncPC(int delta) {
    pc_ += delta;
}

int RegFile::GetReg(int reg) const {
    if (reg >= 1 && reg <= 31) {
        return rf_[reg];
    }
    return 0;
}

void RegFile::PutReg(int reg, int value) {
    if (reg >= 1 && reg <= 31) {
        rf_[reg] = value;
    }
}

void RegFile::Dump() const {
    printf("pc:%08x 01:%08x 02:%08x 03:%08x 04:%08x 05:%08x 06:%08x 07:%08x\n",
	   pc_, rf_[1], rf_[2], rf_[3], rf_[4], rf_[5], rf_[6], rf_[7]);
    printf("08:%08x 09:%08x 10:%08x 11:%08x 12:%08x 13:%08x 14:%08x 15:%08x\n",
	   rf_[8], rf_[9], rf_[10], rf_[11], rf_[12], rf_[13], rf_[14], rf_[15]);
    printf("16:%08x 17:%08x 18:%08x 19:%08x 20:%08x 21:%08x 22:%08x 23:%08x\n",
	   rf_[16], rf_[17], rf_[18], rf_[19], rf_[20], rf_[21], rf_[22], rf_[23]);
    printf("24:%08x 25:%08x 26:%08x 27:%08x 28:%08x 29:%08x 30:%08x 31:%08x\n",
	   rf_[24], rf_[25], rf_[26], rf_[27], rf_[28], rf_[29], rf_[30], rf_[31]);
}

}  // namespace risc_v
