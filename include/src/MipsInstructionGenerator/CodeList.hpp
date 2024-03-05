#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "src/common/utils.hpp"

class CodeList {
public:
    std::vector<std::pair<std::string, bool>> code_list;
    /**
     *
     * @param str
     * @param top 输出的时候是否顶格
     */
    void emplace_back(const std::string& str, bool top = false) {
        code_list.emplace_back(str, top);
    }
    void display(const std::string& path = "", bool show_in_terminal = true) {
        if (show_in_terminal) {
            for(const auto&  [item, top] : code_list) {
                if (top) {
                    std::cout << item << std::endl;
                } else {
                    std::cout << "\t" << item << std::endl;
                }
            }
        }

        if (!path.empty()) {
            std::ofstream fout(path);
            for(const auto&  [item, top] : code_list) {
                if (top) {
                    fout << item << std::endl;
                } else {
                    fout << "\t" << item << std::endl;
                }
            }
        }
    }
    void append(const CodeList& list) {
        code_list.insert(code_list.end(), list.code_list.begin(), list.code_list.end());
    }
};