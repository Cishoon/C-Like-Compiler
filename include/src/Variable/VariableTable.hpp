#pragma once

#include <unordered_map>
#include <cstring>
#include <string>
#include <iostream>
#include <any>
#include <utility>


#include "Variable.hpp"

class VariableTable {
    std::vector<Variable> variables;
public:
    void pop_until_deep(size_t deep = 0) {
        while (!variables.empty() && variables.back().deep > deep) {
            variables.pop_back();
        }
    }

    void add_item(const std::string &name, const VariableType &type, const size_t &deep, const int& param = -1) {
        size_t offset = 0;
        size_t length = 4;
        size_t index = 0;
        for (auto &item: variables) {
            if (item.deep <= deep) {
                offset = item.offset + item.length;
                index++;
            } else {
                break;
            }
        }
        auto new_item = Variable(name, type, deep, param, length, offset);
        if (index == variables.size()) {
            variables.emplace_back(new_item);
        } else {
            variables[index] = new_item;
        }
    }

    [[nodiscard]] std::optional<Variable> lookup(const std::string& name) const {
        for (auto it = variables.rbegin(); it != variables.rend(); ++it) {
            if (it->name == name) {
                return *it;
            }
        }
        return std::nullopt;
    }
private:
};