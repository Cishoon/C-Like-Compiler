#include "MipsInstructionGenerator.hpp"

void MipsInstructionGenerator::generator() {
    for (const auto &[index, quater]: quater_list) {
        if (quater.is_jump()) {

        } else if (quater.op == "=") {
            handleAssign(quater);
        }
    }
}

void MipsInstructionGenerator::handleAssign(const Quater &quater) {
    /* (=, t, _, a)
     * (=, #1, _, a)
     * (=, 1, _, a)
     * 目标一定是一个变量表中的变量
     * 原始值可能是变量表中的变量、临时变量、立即数
     */
    const std::string &arg1 = quater.arg1, &arg2 = quater.arg2, &result = quater.result;

    std::string res;
    if (quater.meta.contains("result")) {
        auto tmp = quater.meta.get<VariableTableItem>("result");
        std::stringstream ss;
        ss << "-" << tmp.offset << "($sp)";
        res = ss.str();
        dlog(res);
    }

    if (Quater::is_number(arg1)) {
        std::stringstream ss;
        ss << "li " << res << ", " << arg1;
        dlog(ss.str());
    }
}
