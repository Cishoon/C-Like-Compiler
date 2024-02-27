#include "Compiler.hpp"

Compiler::Compiler(const std::string &grammar_file_path, const std::string &input_file_path) {
    /* 设置输入文件 */
    std::ifstream file(input_file_path);
    if (!file.is_open()) {
        std::cerr << "无法打开输入文件: " << input_file_path << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    lexer.setInput(buffer.str());
    file.close();

    /* 设置文法文件 */
    fs::path grammar_path(grammar_file_path);
    const std::string cache_file_base = grammar_path.replace_extension("").string();
    const std::string cache_file_path = cache_file_base + ".cache";
    const std::string cache_time_path = cache_file_base + ".cache.time";

    if (needsReParsing(grammar_file_path, cache_time_path)) {
        parser = LR1Parser(grammar_file_path);
        parser.save_tables(cache_file_path);
        saveLastModifiedTime(grammar_file_path, cache_time_path);
    } else {
        parser.load_tables(cache_file_path);
    }
}

bool Compiler::needsReParsing(const std::string &grammar_file_path, const std::string &cache_time_path) {
    auto grammar_last_write_time = fs::last_write_time(grammar_file_path);

    // 检查缓存时间戳文件是否存在
    if (!fs::exists(cache_time_path)) {
        return true; // 缓存时间戳文件不存在，需要重新解析
    }

    std::ifstream time_file(cache_time_path);
    std::time_t cached_time;
    time_file >> cached_time;
    if (time_file.fail() || fileTimeToTimeT(grammar_last_write_time) > cached_time) {
        return true; // 缓存无效或文法文件更新了，需要重新解析
    }

    return false; // 缓存有效，无需重新解析
}

void Compiler::saveLastModifiedTime(const std::string &grammar_file_path, const std::string &time_file_path) {
    auto last_write_time = fs::last_write_time(grammar_file_path);
    std::ofstream time_file(time_file_path);
    time_file << fileTimeToTimeT(last_write_time);
}

std::time_t Compiler::fileTimeToTimeT(const std::filesystem::file_time_type &ftime) {
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
    return std::chrono::system_clock::to_time_t(sctp);
}

void Compiler::compile() {
    parser.stateStack.push(0);

    Token token;
    do {
        token = lexer.getNextToken();

        Symbol currentSymbol(SymbolType::Terminal, token.type_to_string(), token.value);
//        std::cout << currentSymbol.to_string() << std::endl;

        bool shouldContinue = true;
        while (shouldContinue) {
            Action action = parser.get_next_action(currentSymbol);
//            std::cout << action.to_string() << std::endl;
            switch (action.type) {
                case Action::Type::SHIFT: {
                    shouldContinue = false;

                    semanticAnalyzer.updateShiftSymbol(currentSymbol);

                    parser.stateStack.push(action.number);
                    parser.symbolStack.push(currentSymbol);
                    break;
                }
                case Action::Type::REDUCE: {
                    std::vector<Symbol> rhs;
                    Symbol lhs = action.production.lhs;

                    for (size_t i = 0; i < action.production.rhs.size(); ++i) {
                        rhs.insert(rhs.begin(), parser.symbolStack.top());
                        parser.symbolStack.pop();
                        parser.stateStack.pop();
                    }
                    semanticAnalyzer.doSemanticAction(lhs, rhs);

                    parser.symbolStack.push(lhs);
                    int nextState = parser.gotoTable.at({parser.stateStack.top(), action.production.lhs});
                    parser.stateStack.push(nextState);
                    break;
                }
                case Action::Type::ACCEPT:
                    std::cout << "编译成功" << std::endl;
                    return;
                default:
                    std::cerr << "编译失败" << std::endl;
                    return;
            }
        }
    } while(token.type != T_EOF);
}
