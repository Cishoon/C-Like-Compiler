//
// Created by Cishoon on 2024/2/9.
//

#include "VariableTable.hpp"

#include <utility>
//
//void VariableTable::set(const std::string &name, VariableTableItem item) {
//    if (!contain(name)) {
//        std::cerr << "变量表设置失败，不存在名为 " << name << " 的变量" << std::endl;
//        exit(6);
//    }
//
//    table[name] = std::move(item);
//}
//
//void VariableTable::insert(const std::string &name, VariableTableItem item) {
//    if (contain(name)) {
//        std::cerr << "变量表插入失败，已存在名为 " << name << " 的变量" << std::endl;
//        exit(4);
//    }
//
//    table[name] = std::move(item);
//}
//
//bool VariableTable::contain(const std::string &name) const {
//    return table.find(name) != table.end();
//}
//
//VariableTableItem VariableTable::get(const std::string& name) const {
//    if (!contain(name)) {
//        std::cerr << "变量表查询失败，不存在名为 " << name << " 的变量" << std::endl;
//        exit(5);
//    }
//
//    return table.at(name);
//}
//
