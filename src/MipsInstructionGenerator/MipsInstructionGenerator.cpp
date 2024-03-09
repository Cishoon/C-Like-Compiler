#include "src/MipsInstructionGenerator/MipsInstructionGenerator.hpp"

const std::string MipsInstructionGenerator::TEMP_REG_PREFIX = "$t";

CodeList MipsInstructionGenerator::generate_header() {
    CodeList codeList;
    codeList.emplace_back(".data", true);
    codeList.emplace_back(R"(_prompt: .asciiz "Enter an integer:")");
    codeList.emplace_back(R"(_ret: .asciiz "\n")");
    codeList.emplace_back(".globl main", true);
    codeList.emplace_back(".text", true);

    codeList.emplace_back("jal main");
    codeList.emplace_back("li $v0, 10");
    codeList.emplace_back("syscall");

    codeList.emplace_back("\nread:", true);
    codeList.emplace_back("li $v0, 4");
    codeList.emplace_back("la $a0, _prompt");
    codeList.emplace_back("syscall");
    codeList.emplace_back("li $v0, 5");
    codeList.emplace_back("syscall");
    codeList.emplace_back("jr $ra");
    codeList.emplace_back("\nwrite:", true);
    codeList.emplace_back("li $v0, 1");
    codeList.emplace_back("syscall");
    codeList.emplace_back("li $v0, 4");
    codeList.emplace_back("la $a0, _ret");
    codeList.emplace_back("syscall");
    codeList.emplace_back("move $v0, $0");
    codeList.emplace_back("jr $ra");
    return codeList;
}

void MipsInstructionGenerator::generator() {
    if (!func_name.empty()) {
        code_list.emplace_back("\n" + func_name + ":", true);
        code_list.emplace_back("move $fp, $sp");
    }

    for (const auto &[index, quater]: quater_list) {
        if (quater.is_jump()) {
            labels.insert(atoll(quater.result.c_str()));
        }
    }

    for (const auto &[index, quater]: quater_list) {
        // 如果是某个跳转语句目标位置，增加label
        if (labels.find(index) != labels.end()) {
            std::stringstream ss;
            ss << func_name << "_label" << index << ":";
            code_list.emplace_back(ss.str(), true);
        }

        if (quater.is_jump()) {
            // 如果跳转到最后一条语句之后，去掉这条语句的翻译
            if (atoll(quater.result.c_str()) >= quater_list.size()) continue;
            handleJump(quater, index);
        } else if (quater.op == "=") {
            handleAssign(quater, index);
        } else if (quater.op == "+") {
            handleAdd(quater, index);
        } else if (quater.op == "-") {
            handleMinus(quater, index);
        } else if (quater.op == "*") {
            handleMul(quater, index);
        } else if (quater.op == "/") {
            handleDiv(quater, index);
        } else if (quater.op == "%") {
            handleMod(quater, index);
        } else if (quater.op == "return") {
            handleReturn(quater, index);
        } else if (quater.op == "param") {
            handleParam(quater, index);
        } else if (quater.op == "call") {
            handleCall(quater, index);
        }
    }

    auto stack_depth = registerManager.get_stack_pointer();
    if (stack_depth != 0) {
        // 分配局部变量空间
        code_list.emplace_front("addi $sp, $sp, -" + std::to_string(stack_depth));
        // 在每一个jr之前，释放局部变量空间
        for (auto it = code_list.code_list.begin(); it != code_list.code_list.end(); it++) {
            const auto& item = *it;
            if (UTILS::start_with(item.code, "jr")) {
                code_list.code_list.insert(it, CodeBlock("addi $sp, $sp, " + std::to_string(stack_depth)));
                it++;
            }
        }
    }
}

std::pair<size_t, size_t> MipsInstructionGenerator::get_interval(const std::string &var_id) {
    // 获取一个变量的生命周期
    size_t start_point = -1, end_point = -1;

    // 获取开始时间
    for (const auto &[index, quater]: quater_list) {
        if (!quater.meta.contains("result")) continue;

        auto var_res = quater.meta.get<Variable>("result");
        if (var_id == var_res.id) {
            start_point = index;
            break;
        }
    }

    // 获取结束时间
    for (auto it = quater_list.rbegin(); it != quater_list.rend(); it++) {
        const auto &[index, quater] = *it;

        if (quater.meta.contains("arg1")) {
            auto arg1 = quater.meta.get<Variable>("arg1");
            if (var_id == arg1.id) {
                end_point = index;
                break;
            }
        }

        if (quater.meta.contains("arg2")) {
            auto arg2 = quater.meta.get<Variable>("arg2");
            if (var_id == arg2.id) {
                end_point = index;
                break;
            }
        }
    }

    return {start_point, end_point};
}

void MipsInstructionGenerator::handleAssign(const Quater &quater, const size_t &index) {
    /* (=, t, _, a)
     * (=, #1, _, a)
     * (=, 1, _, a)
     * 目标一定是一个变量表中的变量
     * 原始值可能是变量表中的变量、临时变量、立即数
     *
     * 每次要使用一个变量的时候，先将这个变量
     */
    const std::string &arg1 = quater.arg1, &arg2 = quater.arg2, &result = quater.result;

    // 给res变量分配寄存器
    Avalue a_res;
    if (quater.meta.contains("result")) {
        auto var_res = quater.meta.get<Variable>("result");
        auto [start_point, end_point] = get_interval(var_res.id);

        a_res = registerManager.alloc(var_res, start_point, end_point, index);
    }

    std::stringstream ss;

    if (Quater::is_number(arg1)) {
        ss << "li " << tReg(a_res) << ", " << arg1;
        code_list.emplace_back(ss.str());
    } else {
        auto var_arg1 = quater.meta.get<Variable>("arg1");

        auto [start_point, end_point] = get_interval(var_arg1.id);
        Avalue a_arg1 = registerManager.alloc(var_arg1, start_point, end_point, index);


        // (=, param0, _, a)
        // (=, param4, _, a)
        // 如果是第4个以上形参，从栈里读取; 否则放到寄存器里
        if (a_arg1.param >= 4) {
            ss << "lw " << tReg(a_res) << ", " << 4 * (a_arg1.param - 4) << "($fp)";
            code_list.emplace_back(ss.str());
        } else {
            ss << "move " << tReg(a_res) << ", " << tReg(a_arg1);
            code_list.emplace_back(ss.str());
        }
    }
}


void MipsInstructionGenerator::handleAdd(const Quater &quater, const size_t &index) {
    const std::string &arg1 = quater.arg1, &arg2 = quater.arg2, &result = quater.result;

    std::stringstream ss;
    // 给res变量分配寄存器
    auto [a_arg1, a_arg2] = alloc_variable_pair(quater, arg1, arg2, index, "arg1", "arg2");
    auto a_res = alloc_a_variable(quater, result, index, "result");

    ss << "add " << tReg(a_res) << ", " << tReg(a_arg1) << ", " << tReg(a_arg2);
    code_list.emplace_back(ss.str());
}

void MipsInstructionGenerator::handleMinus(const Quater &quater, const size_t &index) {
    const std::string &arg1 = quater.arg1, &arg2 = quater.arg2, &result = quater.result;

    std::stringstream ss;
    // 给res变量分配寄存器
    auto [a_arg1, a_arg2] = alloc_variable_pair(quater, arg1, arg2, index, "arg1", "arg2");
    auto a_res = alloc_a_variable(quater, result, index, "result");

    ss << "sub " << tReg(a_res) << ", " << tReg(a_arg1) << ", " << tReg(a_arg2);
    code_list.emplace_back(ss.str());
}

void MipsInstructionGenerator::handleMul(const Quater &quater, const size_t &index) {
    const std::string &arg1 = quater.arg1, &arg2 = quater.arg2, &result = quater.result;

    std::stringstream ss;
    // 给res变量分配寄存器
    auto [a_arg1, a_arg2] = alloc_variable_pair(quater, arg1, arg2, index, "arg1", "arg2");
    auto a_res = alloc_a_variable(quater, result, index, "result");

    ss << "mul " << tReg(a_res) << ", " << tReg(a_arg1) << ", " << tReg(a_arg2);
    code_list.emplace_back(ss.str());
}

void MipsInstructionGenerator::handleDiv(const Quater &quater, const size_t &index) {
    const std::string &arg1 = quater.arg1, &arg2 = quater.arg2, &result = quater.result;

    std::stringstream ss;
    // 给res变量分配寄存器
    auto [a_arg1, a_arg2] = alloc_variable_pair(quater, arg1, arg2, index, "arg1", "arg2");
    auto a_res = alloc_a_variable(quater, result, index, "result");

    ss << "div " << tReg(a_res) << ", " << tReg(a_arg1) << ", " << tReg(a_arg2);
    code_list.emplace_back(ss.str());
}

void MipsInstructionGenerator::handleMod(const Quater &quater, const size_t &index) {
    const std::string &arg1 = quater.arg1, &arg2 = quater.arg2, &result = quater.result;

    std::stringstream ss;
    // 给res变量分配寄存器
    auto [a_arg1, a_arg2] = alloc_variable_pair(quater, arg1, arg2, index, "arg1", "arg2");
    auto a_res = alloc_a_variable(quater, result, index, "result");

    ss << "div " << tReg(a_arg1) << ", " << tReg(a_arg2);
    code_list.emplace_back(ss.str());
    code_list.emplace_back("mfhi " + tReg(a_res));
}

void MipsInstructionGenerator::display_code_list(const std::string &file_path) {
    code_list.display(file_path);
}

Avalue MipsInstructionGenerator::alloc_a_variable(const Quater &quater, const std::string &arg, const size_t &index,
                                                  const std::string &meta) {
    Avalue a_arg;
    if (Quater::is_number(arg)) {
        auto var_arg = Variable(arg, VariableType::INT, 0, -1, 4);
        a_arg = registerManager.alloc(var_arg, index, index, index);
        std::stringstream ss;
        ss << "li " << tReg(a_arg) << ", " << arg;
        code_list.emplace_back(ss.str());
    } else {
        auto var_arg = quater.meta.get<Variable>(meta);
        auto [start_point, end_point] = get_interval(var_arg.id);
        a_arg = registerManager.alloc(var_arg, start_point, end_point, index);
    }
    return a_arg;
}

std::pair<Avalue, Avalue>
MipsInstructionGenerator::alloc_variable_pair(const Quater &quater, const std::string &arg1, const std::string &arg2,
                                              const size_t &index,
                                              const std::string &meta1, const std::string &meta2) {
    Variable var_arg1 = Quater::is_number(arg1) ? Variable(arg1, VariableType::INT, 0, -1, 4)
                                                : quater.meta.get<Variable>(
                    meta1);
    Variable var_arg2 = Quater::is_number(arg2) ? Variable(arg2, VariableType::INT, 0, -1, 4)
                                                : quater.meta.get<Variable>(
                    meta2);

    size_t start_point1 = Quater::is_number(arg1) ? index : get_interval(var_arg1.id).first;
    size_t start_point2 = Quater::is_number(arg2) ? index : get_interval(var_arg2.id).first;
    size_t end_point1 = Quater::is_number(arg1) ? index : get_interval(var_arg1.id).second;
    size_t end_point2 = Quater::is_number(arg2) ? index : get_interval(var_arg2.id).second;

    auto [a_arg1, a_arg2] = registerManager.allocPair(var_arg1, start_point1, end_point1, var_arg2, start_point2,
                                                      end_point2, index);

    if (Quater::is_number(arg1)) {
        std::stringstream ss;
        ss << "li " << tReg(a_arg1) << ", " << arg1;
        code_list.emplace_back(ss.str());
    }

    if (Quater::is_number(arg2)) {
        std::stringstream ss;
        ss << "li " << tReg(a_arg2) << ", " << arg2;
        code_list.emplace_back(ss.str());
    }

    return {a_arg1, a_arg2};
}

CodeList MipsInstructionGenerator::get_code_list() {
    return code_list;
}

void MipsInstructionGenerator::handleReturn(const Quater &quater, const size_t &index) {
    const std::string &arg1 = quater.arg1, &arg2 = quater.arg2, &result = quater.result;

    // 复制 $v0, 返回寄存器
    if (!arg1.empty()) {
        std::stringstream ss;

        if (Quater::is_number(arg1)) {
            ss << "li " << "$v0" << ", " << arg1;
            code_list.emplace_back(ss.str());
        } else {
            auto var_arg1 = quater.meta.get<Variable>("arg1");
            auto [start_point, end_point] = get_interval(var_arg1.id);

            Avalue a_arg1 = registerManager.alloc(var_arg1, start_point, end_point, index);

            ss << "move " << "$v0" << ", " << tReg(a_arg1);
            code_list.emplace_back(ss.str());
        }
    }

    code_list.emplace_back("jr $ra");
}

void MipsInstructionGenerator::handleParam(const Quater &quater, const size_t &index) {
    //  1      param   #2          0
    //  2      param   2           1
    /*
     * 前四个参数直接放到 $a0 - $a3 中，后续的
     */
    const std::string &arg1 = quater.arg1, &arg2 = quater.arg2, &result = quater.result;
    if (strtol(result.c_str(), nullptr, 10) >= 4) {
        // std::cerr << "暂时未实现传参数量大于4的" << std::endl;
        // 到call里保存现场之后压栈

        return;
    }

    std::stringstream ss;
    if (Quater::is_number(arg1)) {
        ss << "li $a" << result << ", " << arg1;
    } else {
        auto a_arg1 = alloc_a_variable(quater, arg1, index, "arg1");
        ss << "move $a" << result << ", " << tReg(a_arg1);
    }
    code_list.emplace_back(ss.str());
}

void MipsInstructionGenerator::handleCall(const Quater &quater, const size_t &index) {
    // arg1 里是调用的函数名，result是返回的结果存在哪里，如果为空表示没有返回
    const std::string &arg1 = quater.arg1, &arg2 = quater.arg2, &result = quater.result;
    const auto &args = quater.meta.get<std::vector<Variable>>("args");

    // 保存现场
    save_context(index);

    // 保存超出4个的参数
    // 保存现场后，所有临时寄存器都可以直接使用，所以压栈使用的中间寄存器都直接用$t0即可
    if (args.size() > 4) {
        for (auto i = args.size() - 1; i >= 4; i--) {
            auto &arg = args[i];
            std::stringstream ss;
            if (Quater::is_number(arg.name)) {
                ss << "li $t0, " << arg.name;
                code_list.emplace_back(ss.str());
                stackManager.push("$t0");
            } else {
                auto a_arg = registerManager.get_variable(arg.id);
                if (a_arg.load) {
                    stackManager.push(tReg(a_arg));
                } else {
                    ss << "lw $t0, " << a_arg.offset << "($fp)";
                    code_list.emplace_back(ss.str());
                    stackManager.push("$t0");
                }
            }
        }
    }

    // 调用函数
    code_list.emplace_back("jal " + arg1);

    // 弹出形参
    if (args.size() > 4) {
        code_list.emplace_back("addi $sp, $sp, " + std::to_string((args.size() - 4) * 4));
    }

    // 恢复现场
    load_context();

    // 将返回结果存下来
    if (!result.empty()) {
        auto a_res = alloc_a_variable(quater, result, index, "result");
        std::stringstream ss;
        ss << "move " << tReg(a_res) << ", $v0";
        code_list.emplace_back(ss.str());
    }
}

void MipsInstructionGenerator::save_context(const size_t &current_point) {
    registerManager.spill_all(current_point); // 把所有寄存器清空
    stackManager.push_list(context_list);
    // for (auto &it: context_list) {
    //     stackManager.push(it);
    // }
}

void MipsInstructionGenerator::load_context() {
    stackManager.pop_list(context_list);
    registerManager.reserve_all();
    // for (auto it = context_list.rbegin(); it != context_list.rend(); it++) {
    //     stackManager.pop(*it);
    // }
}

void MipsInstructionGenerator::handleJump(const Quater &quater, const size_t &index) {
    /*
     *
     */
    const std::string &arg1 = quater.arg1, &arg2 = quater.arg2, &result = quater.result;

    std::stringstream ss;

    if (quater.op == "j") {
        ss << "j " << func_name << "_label" << result;
        code_list.emplace_back(ss.str());
        return;
    }

    auto [a_arg1, a_arg2] =
            alloc_variable_pair(quater, arg1, arg2, index, "arg1", "arg2");

    if (quater.op == "j<") {
        ss << "blt " << tReg(a_arg1) << ", " << tReg(a_arg2) << ", " << func_name << "_label" << result;
    } else if (quater.op == "j>") {
        ss << "bgt " << tReg(a_arg1) << ", " << tReg(a_arg2) << ", " << func_name << "_label" << result;
    } else if (quater.op == "j<=") {
        ss << "ble " << tReg(a_arg1) << ", " << tReg(a_arg2) << ", " << func_name << "_label" << result;
    } else if (quater.op == "j>=") {
        ss << "bge " << tReg(a_arg1) << ", " << tReg(a_arg2) << ", " << func_name << "_label" << result;
    } else if (quater.op == "j==") {
        ss << "beq " << tReg(a_arg1) << ", " << tReg(a_arg2) << ", " << func_name << "_label" << result;
    } else if (quater.op == "j!=") {
        ss << "bne " << tReg(a_arg1) << ", " << tReg(a_arg2) << ", " << func_name << "_label" << result;
    }
    code_list.emplace_back(ss.str());
}
