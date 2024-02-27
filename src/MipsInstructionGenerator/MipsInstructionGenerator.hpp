#pragma once

#include "Semantic/QuaterList.hpp"
#include <vector>
#include "MipsInstruction.hpp"

class MipsInstructionGenerator {
    const QuaterList &quater_list;
    std::vector<MipsInstruction> mips_code;


public:
    explicit MipsInstructionGenerator(const QuaterList &quaterList) : quater_list(quaterList) {}

    void generator();

    void handleAssign(const Quater& quater);
};


