#pragma once

#include <string>
#include <utility>
#include "TokenType.hpp"


struct Token {
    Token(const TokenType &type, std::string value) : type(type), value(std::move(value)) {}

    Token() { type = T_UNKNOWN; };

    TokenType type;     // 属于哪个终结符
    std::string value;  // 终结符的具体值

    [[nodiscard]] std::string type_to_string() const {
        return tokenTypeToString(type);
    }

public:
    static std::string tokenTypeToString(const TokenType &type) {
        switch (type) {
            case T_IDENTIFIER:
                return "T_IDENTIFIER";
            case T_INTEGER_LITERAL:
                return "T_INTEGER_LITERAL";
            case T_FLOAT_LITERAL:
                return "T_FLOAT_LITERAL";
            case T_STRING_LITERAL:
                return "T_STRING_LITERAL";
            case T_CHAR_LITERAL:
                return "T_CHAR_LITERAL";
            case T_IF:
                return "T_IF";
            case T_ELSE:
                return "T_ELSE";
            case T_WHILE:
                return "T_WHILE";
            case T_FOR:
                return "T_FOR";
            case T_RETURN:
                return "T_RETURN";
            case T_INT:
                return "T_INT";
            case T_FLOAT:
                return "T_FLOAT";
            case T_CHAR:
                return "T_CHAR";
            case T_VOID:
                return "T_VOID";
            case T_STRUCT:
                return "T_STRUCT";
            case T_PLUS:
                return "T_PLUS";
            case T_MINUS:
                return "T_MINUS";
            case T_MULTIPLY:
                return "T_MULTIPLY";
            case T_DIVIDE:
                return "T_DIVIDE";
            case T_ASSIGN:
                return "T_ASSIGN";
            case T_EQUAL:
                return "T_EQUAL";
            case T_NOTEQUAL:
                return "T_NOTEQUAL";
            case T_LESS:
                return "T_LESS";
            case T_LESSEQUAL:
                return "T_LESSEQUAL";
            case T_GREATER:
                return "T_GREATER";
            case T_GREATEREQUAL:
                return "T_GREATEREQUAL";
            case T_AND:
                return "T_AND";
            case T_OR:
                return "T_OR";
            case T_NOT:
                return "T_NOT";
            case T_MOD:
                return "T_MOD";
            case T_INCREMENT:
                return "T_INCREMENT";
            case T_DECREMENT:
                return "T_DECREMENT";
            case T_BITAND:
                return "T_BITAND";
            case T_BITOR:
                return "T_BITOR";
            case T_BITXOR:
                return "T_BITXOR";
            case T_BITNOT:
                return "T_BITNOT";
            case T_LEFTSHIFT:
                return "T_LEFTSHIFT";
            case T_RIGHTSHIFT:
                return "T_RIGHTSHIFT";
            case T_SEMICOLON:
                return "T_SEMICOLON";
            case T_LEFT_BRACE:
                return "T_LEFT_BRACE";
            case T_RIGHT_BRACE:
                return "T_RIGHT_BRACE";
            case T_LEFT_PAREN:
                return "T_LEFT_PAREN";
            case T_RIGHT_PAREN:
                return "T_RIGHT_PAREN";
            case T_LEFT_SQUARE:
                return "T_LEFT_SQUARE";
            case T_RIGHT_SQUARE:
                return "T_RIGHT_SQUARE";
            case T_COMMA:
                return "T_COMMA";
            case T_DOT:
                return "T_DOT";
            case T_ARROW:
                return "T_ARROW";
            case T_COLON:
                return "T_COLON";
            case T_QUESTION:
                return "T_QUESTION";
            case T_UNKNOWN:
                return "T_UNKNOWN";
            case T_EOF:
                return "T_EOF";
            default:
                return "INVALID_TOKEN_TYPE";
        }
    }
    static TokenType stringToTokenType(const std::string &str) {
        if (str == "T_IDENTIFIER") return T_IDENTIFIER;
        else if (str == "T_INTEGER_LITERAL") return T_INTEGER_LITERAL;
        else if (str == "T_FLOAT_LITERAL") return T_FLOAT_LITERAL;
        else if (str == "T_STRING_LITERAL") return T_STRING_LITERAL;
        else if (str == "T_CHAR_LITERAL") return T_CHAR_LITERAL;
        else if (str == "T_IF") return T_IF;
        else if (str == "T_ELSE") return T_ELSE;
        else if (str == "T_WHILE") return T_WHILE;
        else if (str == "T_FOR") return T_FOR;
        else if (str == "T_RETURN") return T_RETURN;
        else if (str == "T_INT") return T_INT;
        else if (str == "T_FLOAT") return T_FLOAT;
        else if (str == "T_CHAR") return T_CHAR;
        else if (str == "T_VOID") return T_VOID;
        else if (str == "T_STRUCT") return T_STRUCT;
        else if (str == "T_PLUS") return T_PLUS;
        else if (str == "T_MINUS") return T_MINUS;
        else if (str == "T_MULTIPLY") return T_MULTIPLY;
        else if (str == "T_DIVIDE") return T_DIVIDE;
        else if (str == "T_ASSIGN") return T_ASSIGN;
        else if (str == "T_EQUAL") return T_EQUAL;
        else if (str == "T_NOTEQUAL") return T_NOTEQUAL;
        else if (str == "T_LESS") return T_LESS;
        else if (str == "T_LESSEQUAL") return T_LESSEQUAL;
        else if (str == "T_GREATER") return T_GREATER;
        else if (str == "T_GREATEREQUAL") return T_GREATEREQUAL;
        else if (str == "T_AND") return T_AND;
        else if (str == "T_OR") return T_OR;
        else if (str == "T_NOT") return T_NOT;
        else if (str == "T_MOD") return T_MOD;
        else if (str == "T_INCREMENT") return T_INCREMENT;
        else if (str == "T_DECREMENT") return T_DECREMENT;
        else if (str == "T_BITAND") return T_BITAND;
        else if (str == "T_BITOR") return T_BITOR;
        else if (str == "T_BITXOR") return T_BITXOR;
        else if (str == "T_BITNOT") return T_BITNOT;
        else if (str == "T_LEFTSHIFT") return T_LEFTSHIFT;
        else if (str == "T_RIGHTSHIFT") return T_RIGHTSHIFT;
        else if (str == "T_SEMICOLON") return T_SEMICOLON;
        else if (str == "T_LEFT_BRACE") return T_LEFT_BRACE;
        else if (str == "T_RIGHT_BRACE") return T_RIGHT_BRACE;
        else if (str == "T_LEFT_PAREN") return T_LEFT_PAREN;
        else if (str == "T_RIGHT_PAREN") return T_RIGHT_PAREN;
        else if (str == "T_LEFT_SQUARE") return T_LEFT_SQUARE;
        else if (str == "T_RIGHT_SQUARE") return T_RIGHT_SQUARE;
        else if (str == "T_COMMA") return T_COMMA;
        else if (str == "T_DOT") return T_DOT;
        else if (str == "T_ARROW") return T_ARROW;
        else if (str == "T_COLON") return T_COLON;
        else if (str == "T_QUESTION") return T_QUESTION;
        else if (str == "T_EOF") return T_EOF;
        else return T_UNKNOWN; // Assuming INVALID_TOKEN_TYPE is a defined value for invalid inputs
    }

};
