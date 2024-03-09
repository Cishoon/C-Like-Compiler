#include "src/Lexer/Lexer.hpp"
#include <cctype>
#include <iostream>
#include <utility>


char Lexer::peek() {
    if (index < input.size()) {
        return input[index];
    }
    return '\0';
}

char Lexer::getChar() {
    if (input[index] == '\n') {
        row++;
        col = 0;
        pre_line = current_line;
        current_line.clear();
    } else {
        col++;
        current_line += input[index];
    }
    return input[index++];
}

void Lexer::skipWhitespace() {
    while (std::isspace(peek())) {
        getChar();
    }
}

Token Lexer::getIdentifierOrKeyword() {
    std::string value;
    while (std::isalnum(peek()) || peek() == '_') {
        value += getChar();
    }

    if (keywordMap.count(value)) {
        return {keywordMap[value], value};
    }
    return {T_IDENTIFIER, value};
}

Token Lexer::getOperator() {
    std::string value;
    value += getChar();
    if (operatorMap.count(value + peek())) {
        value += getChar();
    }
    if (operatorMap.count(value)) {
        return {operatorMap[value], value};
    }
    return {T_UNKNOWN, value};
}

Token Lexer::getStringLiteral() {
    std::string value;
    value += getChar();  // Consume the opening quote
    while (peek() != '"' && peek() != '\0') {
        value += getChar();
    }
    value += getChar();  // Consume the closing quote
    return {T_STRING_LITERAL, value};
}

Token Lexer::getCharLiteral() {
    std::string value;
    value += getChar();  // Consume the opening quote
    value += getChar();  // Consume the character
    value += getChar();  // Consume the closing quote
    return {T_CHAR_LITERAL, value};
}

Token Lexer::getNumber() {
    std::string value;
    bool isFloat = false;

    while (std::isdigit(peek()) || (!isFloat && peek() == '.')) {
        if (peek() == '.') {
            isFloat = true;
        }
        value += getChar();
    }

    // 检查数字后是否紧跟字母或下划线，如果是，则消耗直到非字母、非数字、非下划线的字符为止
    if (std::isalpha(peek()) || peek() == '_') {
        while (std::isalnum(peek()) || peek() == '_') {
            value += getChar();
        }
        return {T_UNKNOWN, value};
    }

    if (isFloat) {
        return {T_FLOAT_LITERAL, value};
    } else {
        return {T_INTEGER_LITERAL, value};
    }
}

Token Lexer::getDelimiter() {
    std::string value;
    value += getChar();

    // 检查是否有双字符界定符，如 "->"
    if (delimiterMap.count(value + peek())) {
        value += getChar();
    }

    if (delimiterMap.count(value)) {
        return {delimiterMap[value], value};
    }
    return {T_UNKNOWN, value};
}

void Lexer::skipComment() {
    bool hasComment = false;
    if (peek() == '/') {
        if (input[index + 1] == '/') {  // 单行注释
            hasComment = true;
            while (peek() != '\n' && peek() != '\0') {
                getChar();
            }
            if (peek() == '\n') {
                getChar();  // 消耗换行符
            }
        } else if (input[index + 1] == '*') {
            hasComment = true;
            // 多行注释
            getChar();  // 消耗/
            getChar();  // 消耗*
            while (!(peek() == '*' && input[index + 1] == '/')) {
                if (peek() == '\0') {
                    // 提示错误：多行注释没有正确关闭
                    std::cerr << "Error: Unclosed multi-line comment" << std::endl;
                    // exit(2);
                    throw std::exception();
                }
                getChar();
            }
            getChar();  // 消耗*
            getChar();  // 消耗/
        }
    }
    if (hasComment) {
        skipWhitespace();
        skipComment();
    }
}

Lexer::Lexer(std::string input) : input(std::move(input)) {}

Token Lexer::getNextToken() {
    skipWhitespace();
    skipComment();
    if (index >= input.size()) {
        return {T_EOF, ""};
    }

    char c = peek();
    if (std::isalpha(c) || c == '_') {
        return getIdentifierOrKeyword();
    }
    if (c == '"') {
        return getStringLiteral();
    }
    if (c == '\'') {
        return getCharLiteral();
    }
    if (std::isdigit(c) || (c == '.' && index + 1 < input.size() && std::isdigit(input[index + 1]))) {
        return getNumber();
    }
    if (operatorMap.count(std::string(1, c))) {
        return getOperator();
    }
    if (delimiterMap.count(std::string(1, c))) {
        return getDelimiter();
    }


    return {T_UNKNOWN, std::string(1, getChar())};
}

void Lexer::setInput(std::string _input) {
    this->input = std::move(_input);
}

std::string Lexer::getCurrentRowCol() {
    while(input[index] != '\n') {
        current_line+=input[index];
        index++;
    }
    pre_line.back() = '\n';
    current_line.back() = '\n';

    std::stringstream ss;
    ss << "当前位置 row: " << row << " col: " << col << '\n';
    ss << row -1 << ": " << pre_line << row << ": " << current_line;
    for (int i = 0; i < std::to_string(row).size() + 2; i++) {
        ss << " ";
    }
    for (int i = 0; i < col - 1 ; i++) {
        ss << "-";
    }
    ss << "^\n";
    return ss.str();
}
