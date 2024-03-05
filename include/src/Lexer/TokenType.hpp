#pragma once

enum TokenType {
    // 字面量 (Literals)
    T_IDENTIFIER,       // 例如: variableName, 任意其他变量名、函数名
    T_INTEGER_LITERAL,  // 例如: 42
    T_FLOAT_LITERAL,    // 例如: 3.14
    T_STRING_LITERAL,   // 例如: "hello"
    T_CHAR_LITERAL,     // 例如: 'a'

    // 关键字 (Keywords)
    T_IF,      // if
    T_ELSE,    // else
    T_WHILE,   // while
    T_FOR,     // for
    T_RETURN,  // return
    T_INT,     // int
    T_FLOAT,   // float
    T_CHAR,    // char
    T_VOID,    // void
    T_STRUCT,  // struct
    // ... 可以继续添加其他关键字，如: switch, case, etc.

    // 操作符 (Operators)
    T_PLUS,          // +
    T_MINUS,         // -
    T_MULTIPLY,      // *
    T_DIVIDE,        // /
    T_ASSIGN,        // =
    T_EQUAL,         // ==
    T_NOTEQUAL,      // !=
    T_LESS,          // <
    T_LESSEQUAL,     // <=
    T_GREATER,       // >
    T_GREATEREQUAL,  // >=
    T_AND,           // &&
    T_OR,            // ||
    T_NOT,           // !
    T_MOD,           // %
    T_INCREMENT,     // ++
    T_DECREMENT,     // --
    T_BITAND,        // &
    T_BITOR,         // |
    T_BITXOR,        // ^
    T_BITNOT,        // ~
    T_LEFTSHIFT,     // <<
    T_RIGHTSHIFT,    // >>
    // ... 其他操作符

    // 界符 (Delimiters/Punctuators)
    T_SEMICOLON,     // ;
    T_LEFT_BRACE,    // {
    T_RIGHT_BRACE,   // }
    T_LEFT_PAREN,    // (
    T_RIGHT_PAREN,   // )
    T_LEFT_SQUARE,   // [
    T_RIGHT_SQUARE,  // ]
    T_COMMA,         // ,
    T_DOT,           // .
    T_ARROW,         // ->
    T_COLON,         // :
    T_QUESTION,      // ?
    // ... 其他界符

    T_UNKNOWN,  // 未知Token
    T_EOF,      // 文件结束
};