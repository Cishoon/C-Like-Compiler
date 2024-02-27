#pragma once

#include <any>
#include <unordered_map>
#include <string>
#include <iostream>

class MetaData {
private:
    std::unordered_map<std::string, std::any> data;

public:
    MetaData() = default;

    template<typename T>
    void set(const std::string& key, T&& value) {
        data[key] = std::forward<T>(value);
    }

    template<typename T>
    T get(const std::string& key) const {
        try {
            return std::any_cast<T>(data.at(key));
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Type mismatch or key not found");
        }
    }

    // 检查键是否存在
    [[nodiscard]] bool contains(const std::string& key) const {
        return data.find(key) != data.end();
    }
};

//int main() {
//    MetaData meta;
//    meta.set("age", 30);
//    meta.set("name", std::string("John Doe"));
//    meta.set("is_employee", true);
//
//    try {
//        std::cout << "Name: " << meta.get<std::string>("name") << std::endl;
//        std::cout << "Age: " << meta.get<int>("age") << std::endl;
//        std::cout << "Is Employee: " << meta.get<bool>("is_employee") << std::endl;
//    } catch (const std::exception& e) {
//        std::cout << e.what() << std::endl;
//    }
//
//    return 0;
//}
