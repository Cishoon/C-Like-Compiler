#pragma once

#include "Quater.hpp"
#include <vector>
#include "src/common/dlog.hpp"
#include "src/Variable/VariableTable.hpp"
#include "tabulate/table.hpp"


class QuaterList : public std::vector<std::pair<size_t, Quater>> {
private:
    size_t next_quater_id;

public:
    QuaterList() : next_quater_id(0) {}

    // size_t add_quater(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result) {
    //     Quater quater = Quater(op, arg1, arg2, result);
    //     size_t this_id = next_quater_id++;
    //     this->emplace_back(this_id, std::move(quater));
    //     return this_id;
    // }

    size_t add_quater(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result,
               const VariableTable &table) {
        Quater quater(op, arg1, arg2, result);

        // 设置变量表中的变量
        auto v_arg1 = table.lookup(arg1);
        auto v_arg2 = table.lookup(arg2);
        auto v_result = table.lookup(result);
        if (v_arg1.has_value()) {
            quater.meta.set("arg1", v_arg1.value());
        }
        if (v_arg2.has_value()) {
            quater.meta.set("arg2", v_arg2.value());
        }
        if (v_result.has_value()) {
            quater.meta.set("result", v_result.value());
        }

        // 设置临时变量
        if (Quater::is_temp(arg1)) {
            quater.meta.set("arg1", Variable(arg1, VariableType::INT, 0, -1));
        }
        if (Quater::is_temp(arg2)) {
            quater.meta.set("arg2", Variable(arg2, VariableType::INT, 0, -1));
        }
        if (Quater::is_temp(result)) {
            quater.meta.set("result", Variable(result, VariableType::INT, 0, -1));
        }

        size_t this_id = next_quater_id++;
        this->emplace_back(this_id, std::move(quater));
        return this_id;
    }

    size_t add_quater(const size_t &base_id, const Quater &quater) {
        this->emplace_back(base_id, quater);
        return base_id;
    }

    size_t append_quaters(const QuaterList &quaters) {
        size_t base_id = next_quater_id;
        for (const auto &[id, quater]: quaters) {
            if (quater.is_jump()) {
                // 跳转语句不仅id要+base_id, 而且result也要加
                Quater q(quater);

                try {
                    int result_int = std::stoi(q.result);
                    q.result = std::to_string(result_int + base_id);
                } catch (const std::invalid_argument &e) {
                    // If conversion fails, do nothing (q.result is not an integer)
                }

                this->add_quater(id + base_id, q);
                continue;
            }

            this->add_quater(id + base_id, quater);
        }
        next_quater_id += quaters.size();
        return base_id;
    }

    void display(tabulate::Color color = tabulate::Color::blue) const {
        using namespace tabulate;
        Table table;
        // 设置表头
        table.add_row({"Index", "Oper", "Arg1", "Arg2", "Result"});

        // 设置表格的边框和列分隔
        table.format()
                .corner("")
                .border("")
                .column_separator("")
                .font_color(color);

        // 配置每一行，去掉行之间的分隔线
        for (size_t i = 0; i < this->size(); ++i) {
            const auto &[index, quater] = this->at(i);
            table.add_row({std::to_string(index), quater.op, quater.arg1, quater.arg2, quater.result});
            table[i + 1].format()
                    .hide_border_top()
                    .column_separator("");
        }

        std::cout << table << std::endl;
    }

    void set_bool_exit(const vector <size_t> &list, size_t exit_id, size_t base_id = 0) {
        const std::string &exit_id_str = std::to_string(exit_id);
        for (const auto &id: list) {
            auto &[_, quater] = this->at(id + base_id);
            quater.result = exit_id_str;
        }
    }
};
