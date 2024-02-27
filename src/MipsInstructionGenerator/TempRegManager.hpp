#pragma once

#include <vector>
#include <string>
#include "Semantic/QuaterList.hpp"
#include "MipsInstruction.hpp"


struct AvalueItem {
    bool load;          // 是否加载到寄存器中
    size_t reg_id;      // 存放在哪个寄存器中
    size_t start_time;  // 最早使用时间
    size_t end_time;    // 最晚使用时间
};

struct RvalueItem {
    bool busy = false;              // 是否被占用
    std::string var_name;           // 存储的变量名
};

class TempRegManager{
private:
    const QuaterList& quater_list;
public:
    explicit TempRegManager(const QuaterList& quaterList) : quater_list(quaterList) {
        rvalue.resize(MAX_REG_NUM);
    }

    std::string allocReg(const std::string& var_name) {
        // 先尝试找空闲寄存器
        for (size_t index = 0; index < rvalue.size(); index++) {
            auto& item = rvalue[index];
            if (!item.busy) {
                _set_var(var_name, index);

                return TEMP_REG_PREFIX + std::to_string(index);
            }
        }

        // 没有空闲寄存器，寄存器溢出到内存
        auto& _best = avalue.begin()->second;

        for (auto& [_, name] : rvalue) {
            auto& item = avalue[name];
            if ((item.end_time > _best.end_time) ||
                    (item.end_time == _best.end_time && item.start_time < _best.start_time)) {
                _best = item;
            }
        }

        // _best是要被替换的
        _set_var(var_name, _best.reg_id);

        // 修改avalue的信息，该变量不在寄存器
        _best = {false, static_cast<size_t>(-1), _best.start_time, _best.end_time};

        return TEMP_REG_PREFIX + std::to_string(_best.reg_id);
    }

private:
    std::pair<size_t, size_t> _get_start_time_and_end_time(const std::string& var_name);
    void _set_var(const std::string& var_name, const size_t& reg_id);

private:
    const size_t MAX_REG_NUM = 10;
    const std::string TEMP_REG_PREFIX = "$t";
    std::vector<RvalueItem> rvalue; // rvalue[0] 表示第0个临时寄存器$t0中存的变量信息
    std::unordered_map<std::string, AvalueItem> avalue; // avalue["#1"] 表示第1个临时变量的信息
};

std::pair<size_t, size_t> TempRegManager::_get_start_time_and_end_time(const std::string &var_name) {
    size_t start_time, end_time = -1;

    for (const auto& [index, quater] : quater_list) {
        if (quater.result == var_name) {
            start_time = index;
            break;
        }
    }

    for (auto it = quater_list.rbegin(); it != quater_list.rend(); it++ ) {
        const auto& [index, quater] = *it;
        if (quater.arg1 == var_name || quater.arg2 == var_name) {
            end_time = index;
            break;
        }
    }

    return {start_time, end_time};
}

void TempRegManager::_set_var(const std::string &var_name, const size_t &reg_id) {
    rvalue[reg_id] = {true, var_name};
    const auto& [_start, _end] = _get_start_time_and_end_time(var_name);
    avalue[var_name] = {true, reg_id, _start, _end};
}

