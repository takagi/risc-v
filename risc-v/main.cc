#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <variant>

#include "machine.h"

template<class T, class... Types>
constexpr std::optional<T> get_if(const std::variant<Types...>& v) noexcept {
    if (auto p = std::get_if<T>(&v)) {
        return *p;
    } else {
        return std::nullopt;
    }
}

struct TraceCommand {};

using Command = std::variant<TraceCommand>;

int main(int argc, char* argv[]) {
    risc_v::Machine m;

    m.LoadElf("/Users/mtakagi/Desktop/foo.out");

    while(1) {
        std::istringstream iss;
        std::string s, cmd, av1;

        std::cout << "? " << std::flush;
        std::getline(std::cin, s);
        if (std::cin.eof()) break;

        iss = std::istringstream{s};
        iss >> cmd;
        if (!iss) continue;

        if (cmd == "r") {
            m.DumpReg();
            continue;
        }

        if (cmd == "s") {
            int value;
            iss >> std::hex >> value;
            if (!iss) {
                std::cerr << "foo" << cmd << std::endl;
                continue;
            }
            m.SetPC(value);
            continue;
        }

        if (cmd == "t") {
            m.Trace();
            m.DumpReg();
            continue;
        }

        std::cerr << "Invalid command: " << cmd << std::endl;
    }

    return 0;
}
