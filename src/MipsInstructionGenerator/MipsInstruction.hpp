#pragma once

#include <string>
#include <utility>

class MipsInstruction {
public:
    std::string op;
    std::vector<std::string> args;

    explicit MipsInstruction(std::string op, const size_t argc = 1) : op(std::move(op)) {
        if (argc > 0) {
            args.reserve(argc);
        }
    }

    MipsInstruction(std::string op, std::vector<std::string> args) : op(std::move(op)), args(std::move(args)){

    }

    MipsInstruction& set_arg(const std::string& arg, int pos = -1) {
        if (pos == -1) {
            args.emplace_back(arg);
        } else if (pos >= 0 && pos <= 2){
            args[pos] = arg;
        }
        return *this;
    }
};
