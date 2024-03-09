#include "src/Lexer/Lexer.hpp"
#include "src/Compiler/Compiler.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "用法: " << argv[0] << "<输入文件> <输出位置> [-s 展示输出结果]" << std::endl;
        return 1;
    }

    std::string grammarFile = "../grammar/grammar.txt";
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    bool show_output = argc == 4;

    Compiler compiler(grammarFile, inputFile, outputFile, show_output);
    compiler.compile();

    return 0;
}
