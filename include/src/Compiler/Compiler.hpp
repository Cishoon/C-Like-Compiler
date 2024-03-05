#pragma once

#include "src/Lexer/Lexer.hpp"
#include "src/LR1Parser/LR1Parser.hpp"
#include "src/Semantic/SemanticAnalyzer.hpp"

namespace fs = std::filesystem;

class Compiler {
private:
    std::string output_file_path;
public:
    explicit Compiler(const std::string &grammar_file_path, const std::string &input_file_path, std::string output_file_path);

    void compile();

private:
    Lexer lexer;
    LR1Parser parser;
    SemanticAnalyzer semanticAnalyzer;


private:
    static bool needsReParsing(const std::string &grammar_file_path, const std::string &cache_time_path);

    static void saveLastModifiedTime(const std::string &grammar_file_path, const std::string &time_file_path);

    static std::time_t fileTimeToTimeT(const fs::file_time_type &ftime);
};


