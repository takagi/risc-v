#pragma once

namespace risc_v {

class RegFile {
public:
    RegFile();

    int GetPC() const;
    void PutPC(int value);
    void IncPC(int delta);

    int GetReg(int reg) const;
    void PutReg(int reg, int value);

    void Dump() const;

private:
    int pc_;
    int rf_[32];
};

}  // namespace risc_v
