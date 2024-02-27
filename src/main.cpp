#include "Lexer/Lexer.hpp"
#include "Compiler.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "用法: " << argv[0] << " <文法文件> <输入文件>" << std::endl;
        return 1;
    }

    std::string grammarFile = argv[1];
    std::string inputFile = argv[2];

    Compiler compiler(grammarFile, inputFile);
    compiler.compile();

    return 0;
}
