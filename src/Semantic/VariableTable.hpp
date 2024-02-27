#pragma once

#include <unordered_map>
#include <cstring>
#include <string>
#include <iostream>
#include <any>
#include <utility>

enum VariableType {
    VOID, BOOL, CHAR, INT, FLOAT
};

struct VariableTableItem {
    std::string name; // 变量名
    VariableType type; // 变量类型
//    std::string value; // 变量初始值 （即将舍弃，

    size_t offset; // 偏移量
    size_t length; // 变量长度

    size_t deep; // 层级，0表示全局变量

    // 构造函数，用于方便地创建 VariableTableItem
//    explicit VariableTableItem(VariableType type = VOID, std::string value = "", size_t length = 4) : type(type), value(std::move(value)), length(length) {}
    explicit VariableTableItem(std::string name, const VariableType &type, const size_t &deep, const size_t &length = 0,
                               const size_t &offset = 0)
            : name(std::move(name)), type(type), offset(offset), length(length), deep(deep) {}
};

//class VariableTable {
//    std::unordered_map<std::string, VariableTableItem> table;
//public:
//    [[nodiscard]] bool contain(const std::string &name) const;
//
//    void insert(const std::string &name, VariableTableItem item);
//
//    void set(const std::string &name, VariableTableItem item);
//
//    [[nodiscard]] VariableTableItem get(const std::string &name) const;
//};

class VariableTable : public std::vector<VariableTableItem> {
public:
    void pop_until_deep(const size_t& deep = 0) {
        while(this->back().deep > deep) {
            this->pop_back();
        }
    }

    void add_item(const std::string &name, const VariableType &type, const size_t &deep) {
        size_t offset = 0;
        size_t length = 4;
        size_t index = 0;
        for (auto &item: *this) {
            if (item.deep <= deep) {
                offset = item.offset + item.length;
                index++;
            } else {
                break;
            }
        }
        auto new_item = VariableTableItem(name, type, deep, length, offset);
        if (index == this->size()) {
            this->emplace_back(new_item);
        } else {
            (*this)[index] = new_item;
        }
    }

    [[nodiscard]] std::optional<VariableTableItem> lookup(const std::string& name) const {
        for (auto it = this->rbegin(); it != this->rend(); it++) {
            if (it->name == name) {
                return *it;
            }
        }
        return {};
    }
private:

};