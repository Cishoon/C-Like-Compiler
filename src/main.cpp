#include "src/Lexer/Lexer.hpp"
#include "src/Compiler/Compiler.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 4) {
        std::cerr << "用法: " << argv[0] << " <文法文件> <输入文件> <输出位置>" << std::endl;
        return 1;
    }

    std::string grammarFile = argv[1];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];

    Compiler compiler(grammarFile, inputFile, outputFile);
    compiler.compile();

    return 0;
}
