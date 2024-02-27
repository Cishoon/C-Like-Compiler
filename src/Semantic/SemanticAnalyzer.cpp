#include "SemanticAnalyzer.hpp"
#include "common/dlog.hpp"
#include "MipsInstructionGenerator/MipsInstructionGenerator.hpp"


void SemanticAnalyzer::updateShiftSymbol(Symbol &symbol) {
    switch (Token::stringToTokenType(symbol.literal)) {
        case T_LEFT_BRACE:
            variable_deep++;
            break;
        case T_RIGHT_BRACE:
            if (variable_deep > 0) {
                variable_deep--;
                variable_table.pop_until_deep(variable_deep);
            }
            break;
        default:
            break;
    }
}

void SemanticAnalyzer::doSemanticAction(Symbol &lhs, const std::vector<Symbol> &rhs) {
    if (lhs.literal == "<Program>") {
        handleProgram(lhs, rhs);
    } else if (lhs.literal == "<declaration_list>") {
        handleDeclarationList(lhs, rhs);
    } else if (lhs.literal == "<declaration>") {
        handleDeclaration(lhs, rhs);
    } else if (lhs.literal == "<var_declaration>") {
        handleVarDeclaration(lhs, rhs);
    } else if (lhs.literal == "<fun_declaration>") {
        handleFunDeclaration(lhs, rhs);
    } else if (lhs.literal == "<type_specifier>") {
        handleTypeSpecifier(lhs, rhs);
    } else if (lhs.literal == "<expression_stmt>") {
        handleExpressionStmt(lhs, rhs);
    } else if (lhs.literal == "<simple_type>") {
        handleSimpleType(lhs, rhs);
    } else if (lhs.literal == "<formal_param>") {
        handleFormalParam(lhs, rhs);
    } else if (lhs.literal == "<param_list>") {
        handleParamList(lhs, rhs);
    } else if (lhs.literal == "<param>") {
        handleParam(lhs, rhs);
    } else if (lhs.literal == "<compound_stmt>") {
        handleCompoundStmt(lhs, rhs);
    } else if (lhs.literal == "<stmt_list>") {
        handleStmtList(lhs, rhs);
    } else if (lhs.literal == "<stmt>") {
        handleStmt(lhs, rhs);
    } else if (lhs.literal == "<expression>") {
        handleExpression(lhs, rhs);
    } else if (lhs.literal == "<var>") {
        handleVar(lhs, rhs);
    } else if (lhs.literal == "<simple_expression>") {
        handleSimpleExpression(lhs, rhs);
    } else if (lhs.literal == "<additive_expression>") {
        handleAdditiveExpression(lhs, rhs);
    } else if (lhs.literal == "<term>") {
        handleTerm(lhs, rhs);
    } else if (lhs.literal == "<factor>") {
        handleFactor(lhs, rhs);
    } else if (lhs.literal == "<fun_call>") {
        handleFunCall(lhs, rhs);
    } else if (lhs.literal == "<argument>") {
        handleArgument(lhs, rhs);
    } else if (lhs.literal == "<addop>") {
        handleAddop(lhs, rhs);
    } else if (lhs.literal == "<mulop>") {
        handleMulop(lhs, rhs);
    } else if (lhs.literal == "<relop>") {
        handleRelop(lhs, rhs);
    } else if (lhs.literal == "<selection_stmt>") {
        handleSelectionStmt(lhs, rhs);
    } else if (lhs.literal == "<return_stmt>") {
        handleReturnStmt(lhs, rhs);
    } else if (lhs.literal == "<iteration_stmt>") {
        handleIterationStmt(lhs, rhs);
    } else if (lhs.literal == "<opt_expression_stmt>") {
        handleOptExpressionStmt(lhs, rhs);
    } else if (lhs.literal == "<bool_expression>") {
        handleBoolExpression(lhs, rhs);
    }
}

void SemanticAnalyzer::handleVar(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <var> ::= T_IDENTIFIER | T_IDENTIFIER T_LEFT_SQUARE <expression> T_RIGHT_SQUARE
    if (rhs.size() != 1) {
        std::cerr << "暂时不能解析数组变量" << std::endl;
    }

    lhs.meta.set("name", rhs[0].real_value);
}

void SemanticAnalyzer::handleVarDeclaration(Symbol &lhs, const std::vector<Symbol> &rhs) {
    if (rhs.size() == 3) {
        // <type_specifier> T_IDENTIFIER T_SEMICOLON
        const std::string &name = rhs[1].real_value;
        const auto &type = rhs[0].meta.get<VariableType>("type");
        variable_table.add_item(name, type, variable_deep);
    } else if (rhs.size() == 5) {
        // <type_specifier> T_IDENTIFIER T_ASSIGN <expression> T_SEMICOLON

        const VariableType &type1 = rhs[0].meta.get<VariableType>("type");
        const VariableType &type2 = rhs[3].meta.get<VariableType>("type");
        const std::string &value2 = rhs[3].meta.get<std::string>("value");

        if (type1 < type2) {
            std::cout << "发生截断" << std::endl;
        }
        const std::string &name = rhs[1].real_value;

        variable_table.add_item(name, type1, variable_deep);
    } else {
        std::cerr << "暂时不能定义数组" << std::endl;
    }
}

void SemanticAnalyzer::handleTypeSpecifier(Symbol &lhs, const std::vector<Symbol> &rhs) {
    lhs.meta.set("type", rhs.back().meta.get<VariableType>("type"));
    if (rhs.size() == 2) {
        lhs.meta.set("isConst", true);
    }
}

void SemanticAnalyzer::handleSimpleType(Symbol &lhs, const std::vector<Symbol> &rhs) {
    switch (Token::stringToTokenType(rhs[0].literal)) {
        case T_VOID:
            lhs.meta.set("type", VOID);
            break;
        case T_FLOAT:
            lhs.meta.set("type", FLOAT);
            break;
        case T_INT:
            lhs.meta.set("type", INT);
            break;
        case T_CHAR:
            lhs.meta.set("type", CHAR);
            break;
        default:
            std::cerr << "变量类型不存在：" << rhs[0].literal << std::endl;
            break;
    }
}

void SemanticAnalyzer::handleExpression(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <simple_expression> | <var> T_ASSIGN <expression>
    if (rhs.size() == 1) {
        lhs.meta.set("type", rhs[0].meta.get<VariableType>("type"));
        lhs.meta.set("value", rhs[0].meta.get<std::string>("value"));
        if (rhs[0].meta.contains("code")) {
            lhs.meta.set("code", rhs[0].meta.get<QuaterList>("code"));
        }
    } else if (rhs.size() == 3 && rhs[2].literal == "<expression>") {
        const std::string &name = rhs[0].meta.get<std::string>("name");

        auto item = variable_table.lookup(name);

        if (!item.has_value()) {
            std::cerr << "未定义变量：" << name << std::endl;
            exit(6);
        }
        const VariableType type1 = item.value().type;

        const VariableType &type2 = rhs[2].meta.get<VariableType>("type");
        if (type2 > type1) {
            std::cout << "发生截断" << std::endl;
        }
        lhs.meta.set("type", type1);

        const std::string &value2 = rhs[2].meta.get<std::string>("value");
        lhs.meta.set("value", name);

        const std::string &op = rhs[1].real_value;
        QuaterList code;
        if (rhs[0].meta.contains("code")) {
            const auto &code1 = rhs[0].meta.get<QuaterList>("code");
            code.append_quaters(code1);
        }
        if (rhs[2].meta.contains("code")) {
            const auto &code2 = rhs[2].meta.get<QuaterList>("code");
            code.append_quaters(code2);
        }

        code.add_quater(op, value2, "", name, variable_table);
        lhs.meta.set("code", code);
    } else if (rhs.size() == 3 && rhs[2].literal == "<bool_expression>") {
        const std::string &name = rhs[0].meta.get<std::string>("name");

        auto item = variable_table.lookup(name);
        if (!item.has_value()) {
            std::cerr << "未定义变量：" << name << std::endl;
            exit(6);
        }
        const VariableType type1 = item.value().type;

        const VariableType &type2 = rhs[2].meta.get<VariableType>("type");
        if (type2 > type1) {
            std::cout << "发生截断" << std::endl;
        }
        lhs.meta.set("type", type1);

        const std::string &value2 = getNewTemp();
        lhs.meta.set("value", name);

        const std::string &op = rhs[1].real_value;
        QuaterList code;
        if (rhs[0].meta.contains("code")) {
            const auto &code1 = rhs[0].meta.get<QuaterList>("code");
            code.append_quaters(code1);
        }
        if (rhs[2].meta.contains("code")) {
            auto code2 = rhs[2].meta.get<QuaterList>("code");
            const auto &true_list = rhs[2].meta.get<std::vector<size_t>>("true_list");
            const auto &false_list = rhs[2].meta.get<std::vector<size_t>>("false_list");

            const auto &temp = value2;
            const auto &true_exit = code2.add_quater("=", "true", "", temp, variable_table);
            code2.add_quater("j", "", "", std::to_string(true_exit + 3));
            const auto &false_exit = code2.add_quater("=", "false", "", temp, variable_table);

            code2.set_bool_exit(true_list, true_exit);
            code2.set_bool_exit(false_list, false_exit);
            code.append_quaters(code2);
        }

        code.add_quater(op, value2, "", name, variable_table);
        lhs.meta.set("code", code);
    }
}

void SemanticAnalyzer::handleSimpleExpression(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <additive_expression> | <bool_expression>
    if (rhs.size() == 1) {
        lhs.meta.set("type", rhs[0].meta.get<VariableType>("type"));
        lhs.meta.set("value", rhs[0].meta.get<std::string>("value"));
        if (!rhs[0].meta.contains("code")) return;

        auto code = rhs[0].meta.get<QuaterList>("code");

        if (rhs[0].literal == "<bool_expression>") {
            const auto &true_list = rhs[0].meta.get<std::vector<size_t>>("true_list");
            const auto &false_list = rhs[0].meta.get<std::vector<size_t>>("false_list");

            const auto &temp = rhs[0].meta.get<std::string>("value");
            const auto &true_exit = code.add_quater("=", "true", "", temp, variable_table);
            code.add_quater("j", "", "", std::to_string(true_exit + 3));
            const auto &false_exit = code.add_quater("=", "false", "", temp, variable_table);

            code.set_bool_exit(true_list, true_exit);
            code.set_bool_exit(false_list, false_exit);
        }

        lhs.meta.set("code", code);
    }
}

void SemanticAnalyzer::handleAdditiveExpression(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <term> | <additive_expression> <addop> <term>
    if (rhs.size() == 1) {
        lhs.meta.set("type", rhs[0].meta.get<VariableType>("type"));
        lhs.meta.set("value", rhs[0].meta.get<std::string>("value"));
        if (rhs[0].meta.contains("code")) {
            lhs.meta.set("code", rhs[0].meta.get<QuaterList>("code"));
        }
    } else if (rhs.size() == 3) {
        const std::string &op = rhs[1].meta.get<std::string>("op");
        const VariableType &type1 = rhs[0].meta.get<VariableType>("type");
        const VariableType &type2 = rhs[2].meta.get<VariableType>("type");
        VariableType type = std::max(type1, type2);
        lhs.meta.set("type", type);

        const std::string &value1 = rhs[0].meta.get<std::string>("value");
        const std::string &value2 = rhs[2].meta.get<std::string>("value");
        std::string value = getNewTemp();
        lhs.meta.set("value", value);

        QuaterList code;
        if (rhs[0].meta.contains("code")) {
            const auto &code1 = rhs[0].meta.get<QuaterList>("code");
            code.append_quaters(code1);
        }
        if (rhs[2].meta.contains("code")) {
            const auto &code2 = rhs[2].meta.get<QuaterList>("code");
            code.append_quaters(code2);
        }

        code.add_quater(op, value1, value2, value, variable_table);
        lhs.meta.set("code", code);
    }
}

void SemanticAnalyzer::handleTerm(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <factor> | <term> <mulop> <factor>
    if (rhs.size() == 1) {
        lhs.meta.set("type", rhs[0].meta.get<VariableType>("type"));
        lhs.meta.set("value", rhs[0].meta.get<std::string>("value"));
        if (rhs[0].meta.contains("code")) {
            lhs.meta.set("code", rhs[0].meta.get<QuaterList>("code"));
        }
    } else {
        const std::string &op = rhs[1].meta.get<std::string>("op");
        const VariableType &type1 = rhs[0].meta.get<VariableType>("type");
        const VariableType &type2 = rhs[2].meta.get<VariableType>("type");
        VariableType type = std::max(type1, type2);
        lhs.meta.set("type", type);

        const std::string &value1 = rhs[0].meta.get<std::string>("value");
        const std::string &value2 = rhs[2].meta.get<std::string>("value");
        std::string value = getNewTemp();
        lhs.meta.set("value", value);

        QuaterList code;
        if (rhs[0].meta.contains("code")) {
            const auto &code1 = rhs[0].meta.get<QuaterList>("code");
            code.append_quaters(code1);
        }
        if (rhs[2].meta.contains("code")) {
            const auto &code2 = rhs[2].meta.get<QuaterList>("code");
            code.append_quaters(code2);
        }

        code.add_quater(op, value1, value2, value, variable_table);

        lhs.meta.set("code", code);
    }
}

void SemanticAnalyzer::handleFactor(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // T_LEFT_PAREN <expression> T_RIGHT_PAREN | <var> | <fun_call> | T_INTEGER_LITERAL | T_STRING_LITERAL | T_CHAR_LITERAL | T_FLOAT_LITERAL
    if (rhs.size() == 3) {
        lhs.meta.set("type", rhs[1].meta.get<VariableType>("type"));
        lhs.meta.set("value", rhs[1].meta.get<std::string>("value"));
        if (rhs[1].meta.contains("code")) {
            lhs.meta.set("code", rhs[1].meta.get<QuaterList>("code"));
        }
    } else if (rhs.size() == 1) {
        const std::string &_literal = rhs[0].literal;
        if (_literal == "<var>") {
            auto name = rhs[0].meta.get<std::string>("name");
            auto item = variable_table.lookup(name);
            if (!item.has_value()) {
                std::cerr << "未定义变量：" << name << std::endl;
                exit(6);
            }
            lhs.meta.set("type", item.value().type);
            lhs.meta.set("value", name);
        } else if (_literal == "<fun_call>") {
            std::cerr << "暂时不支持调用函数" << std::endl;
            exit(-1);
        } else if (_literal == "T_INTEGER_LITERAL") {
            lhs.meta.set("type", VariableType::INT);
            lhs.meta.set("value", rhs[0].real_value);
        } else {
            std::cerr << "暂时不支持处理 factor.literal = " << _literal << std::endl;
            exit(-1);
        }
    }

}


std::string SemanticAnalyzer::getNewTemp() {
    return TEMP_PREFIX + std::to_string(++temp_index);
}


void SemanticAnalyzer::handleMulop(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // T_MULTIPLY | T_DIVIDE | T_MOD | T_BITAND | T_BITOR | T_BITXOR | T_LEFTSHIFT | T_RIGHTSHIFT
    lhs.meta.set("op", rhs[0].real_value);
}

void SemanticAnalyzer::handleAddop(Symbol &lhs, const std::vector<Symbol> &rhs) {
    lhs.meta.set("op", rhs[0].real_value);
}

void SemanticAnalyzer::handleRelop(Symbol &lhs, const std::vector<Symbol> &rhs) {
    lhs.meta.set("op", rhs[0].real_value);
}

void SemanticAnalyzer::handleFunDeclaration(Symbol &lhs, const std::vector<Symbol> &rhs) {
    //  <type_specifier> T_IDENTIFIER T_LEFT_PAREN <formal_param> T_RIGHT_PAREN <compound_stmt> |
    //  <type_specifier> T_IDENTIFIER T_LEFT_PAREN T_RIGHT_PAREN <compound_stmt>

    if (rhs.size() == 5) {
        if (rhs[4].meta.contains("code")) {
            const auto &code = rhs[4].meta.get<QuaterList>("code");
            lhs.meta.set("code", code);

            code.display();

            auto mips_generator = MipsInstructionGenerator(code);
            mips_generator.generator();
        }
    } else {
        std::cerr << "还没写可传参的函数" << std::endl;
    }
}

void SemanticAnalyzer::handleProgram(Symbol &lhs, const std::vector<Symbol> &rhs) {

}

void SemanticAnalyzer::handleDeclarationList(Symbol &lhs, const std::vector<Symbol> &rhs) {

}

void SemanticAnalyzer::handleDeclaration(Symbol &lhs, const std::vector<Symbol> &rhs) {

}

void SemanticAnalyzer::handleExpressionStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <expression_stmt> ::= <expression> T_SEMICOLON
    if (rhs[0].meta.contains("code")) {
        const auto &code = rhs[0].meta.get<QuaterList>("code");
        lhs.meta.set("code", code);
    }
}

void SemanticAnalyzer::handleFormalParam(Symbol &lhs, const std::vector<Symbol> &rhs) {

}

void SemanticAnalyzer::handleParamList(Symbol &lhs, const std::vector<Symbol> &rhs) {

}

void SemanticAnalyzer::handleParam(Symbol &lhs, const std::vector<Symbol> &rhs) {

}

void SemanticAnalyzer::handleCompoundStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <compound_stmt> ::= T_LEFT_BRACE <stmt_list> T_RIGHT_BRACE | T_LEFT_BRACE T_RIGHT_BRACE
    if (rhs.size() == 2) return;
    if (rhs[1].meta.contains("code")) {
        const auto &code = rhs[1].meta.get<QuaterList>("code");
        lhs.meta.set("code", code);
    }
}

void SemanticAnalyzer::handleStmtList(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <stmt_list> ::= <stmt> <stmt_list> | <stmt>
    if (rhs.size() == 1) {
        if (rhs[0].meta.contains("code")) {
            const auto &code = rhs[0].meta.get<QuaterList>("code");
            lhs.meta.set("code", code);
        }
    } else if (rhs.size() == 2) {
        QuaterList code;
        if (rhs[0].meta.contains("code")) {
            const auto &code1 = rhs[0].meta.get<QuaterList>("code");
            code.append_quaters(code1);
        }
        if (rhs[1].meta.contains("code")) {
            const auto &code2 = rhs[1].meta.get<QuaterList>("code");
            code.append_quaters(code2);
        }
        lhs.meta.set("code", code);
    }


}

void SemanticAnalyzer::handleStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <stmt> ::= <var_declaration> | <expression_stmt> | <compound_stmt> | <selection_stmt> | <iteration_stmt> | <return_stmt>
    if (rhs[0].meta.contains("code")) {
        const auto &code = rhs[0].meta.get<QuaterList>("code");
        lhs.meta.set("code", code);
    }
}

void SemanticAnalyzer::handleFunCall(Symbol &lhs, const std::vector<Symbol> &rhs) {

}

void SemanticAnalyzer::handleArgument(Symbol &lhs, const std::vector<Symbol> &rhs) {

}

void SemanticAnalyzer::handleSelectionStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // T_IF T_LEFT_PAREN <bool_expression> T_RIGHT_PAREN <stmt> |
    // T_IF T_LEFT_PAREN <bool_expression> T_RIGHT_PAREN <stmt> T_ELSE <stmt>

    if (rhs.size() == 5) {
        auto code = rhs[2].meta.get<QuaterList>("code");
        const auto &true_list = rhs[2].meta.get<std::vector<size_t >>("true_list");
        const auto &false_list = rhs[2].meta.get<std::vector<size_t >>("false_list");

        const auto &code2 = rhs[4].meta.get<QuaterList>("code");

        const auto &true_exit = code.append_quaters(code2);
        code.set_bool_exit(true_list, true_exit);
        code.set_bool_exit(false_list, code.size());

        lhs.meta.set("code", code);
    } else if (rhs.size() == 7) {
        auto code = rhs[2].meta.get<QuaterList>("code");
        const auto &true_list = rhs[2].meta.get<std::vector<size_t >>("true_list");
        const auto &false_list = rhs[2].meta.get<std::vector<size_t >>("false_list");

        const auto &code2 = rhs[4].meta.get<QuaterList>("code");
        const auto &code3 = rhs[6].meta.get<QuaterList>("code");

        const auto &true_exit = code.append_quaters(code2);
        code.add_quater("j", "", "", std::to_string(code.size() + code3.size() + 1));
        const auto &false_exit = code.append_quaters(code3);

        code.set_bool_exit(true_list, true_exit);
        code.set_bool_exit(false_list, false_exit);

        lhs.meta.set("code", code);
    }
}

void SemanticAnalyzer::handleReturnStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {

}

void SemanticAnalyzer::handleIterationStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // T_WHILE T_LEFT_PAREN <bool_expression> T_RIGHT_PAREN <stmt> |
    if (rhs.size() == 5) {
        auto code = rhs[2].meta.get<QuaterList>("code");
        const auto &code2 = rhs[4].meta.get<QuaterList>("code");
        const auto &true_list = rhs[2].meta.get<std::vector<size_t >>("true_list");
        const auto &false_list = rhs[2].meta.get<std::vector<size_t >>("false_list");

        const auto &true_exit = code.append_quaters(code2);
        code.add_quater("j", "", "", "0");

        code.set_bool_exit(true_list, true_exit);
        code.set_bool_exit(false_list, code.size());

        lhs.meta.set("code", code);
    } else if (rhs.size() == 8) {
        // T_FOR T_LEFT_PAREN <可选表达式语句> <布尔表达式> T_SEMICOLON <可选表达式语句> T_RIGHT_PAREN <语句>
        const auto &code1 = rhs[2].meta.get<QuaterList>("code");
        const auto &code2 = rhs[3].meta.get<QuaterList>("code");
        const auto &code3 = rhs[5].meta.get<QuaterList>("code");
        const auto &code4 = rhs[7].meta.get<QuaterList>("code");
        const auto &true_list = rhs[3].meta.get<std::vector<size_t>>("true_list");
        const auto &false_list = rhs[3].meta.get<std::vector<size_t>>("false_list");


        auto code = code1;
        const auto &loop = code.append_quaters(code2);
        const auto &true_exit = code.append_quaters(code4);
        code.append_quaters(code3);
        code.add_quater("j", "", "", std::to_string(loop));
        code.set_bool_exit(true_list, true_exit, loop);
        code.set_bool_exit(false_list, code.size(), loop);

        lhs.meta.set("code", code);
    }
}

void SemanticAnalyzer::handleOptExpressionStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <opt_expression_stmt> ::= <expression_stmt> | T_SEMICOLON
    if (rhs[0].literal == "<expression_stmt>") {
        lhs.meta = rhs[0].meta;
        lhs.meta.set("empty", false);
    } else {
        lhs.meta.set("empty", true);
    }
}

void SemanticAnalyzer::handleBoolExpression(Symbol &lhs, const std::vector<Symbol> &rhs) {
    //<布尔表达式> ::= T_LEFT_PAREN <布尔表达式> T_RIGHT_PAREN |
    // <加法表达式> <关系运算符> <加法表达式> |
    // <布尔表达式> T_AND <布尔表达式> |
    // <布尔表达式> T_OR <布尔表达式> |
    // T_NOT <布尔表达式>
    if (rhs.size() == 3 && rhs[1].literal == "<relop>") {
        const std::string &op = rhs[1].meta.get<std::string>("op");
        lhs.meta.set("type", BOOL);


        const std::string &value1 = rhs[0].meta.get<std::string>("value");
        const std::string &value2 = rhs[2].meta.get<std::string>("value");
//        std::string value = getNewTemp();
//        lhs.meta.set("value", value);

        QuaterList code;
        if (rhs[0].meta.contains("code")) {
            const auto &code1 = rhs[0].meta.get<QuaterList>("code");
            code.append_quaters(code1);
        }
        if (rhs[2].meta.contains("code")) {
            const auto &code2 = rhs[2].meta.get<QuaterList>("code");
            code.append_quaters(code2);
        }

        std::vector<size_t> true_list, false_list;
        auto true_id = code.add_quater("j" + op, value1, value2, "");
        auto false_id = code.add_quater("j", "", "", "");
        true_list.emplace_back(true_id);
        false_list.emplace_back(false_id);

        lhs.meta.set("code", code);
        lhs.meta.set("true_list", true_list);
        lhs.meta.set("false_list", false_list);
    } else if (rhs.size() == 3 && rhs[1].literal == "<bool_expression>") {
        lhs.meta = rhs[1].meta;
    } else if (rhs.size() == 3 && rhs[1].literal == "T_AND") {
        lhs.meta.set("type", BOOL);

        const std::string &value1 = rhs[0].meta.get<std::string>("value");
        const std::string &value2 = rhs[2].meta.get<std::string>("value");
        std::string value = getNewTemp();
        lhs.meta.set("value", value);

        std::vector<size_t> true_list, false_list;
        auto true_list1 = rhs[0].meta.get<std::vector<size_t>>("true_list");
        auto true_list2 = rhs[2].meta.get<std::vector<size_t>>("true_list");
        auto false_list1 = rhs[0].meta.get<std::vector<size_t>>("false_list");
        auto false_list2 = rhs[2].meta.get<std::vector<size_t>>("false_list");

        QuaterList code, code1, code2;
        if (rhs[0].meta.contains("code")) {
            code1 = rhs[0].meta.get<QuaterList>("code");
        }
        if (rhs[2].meta.contains("code")) {
            code2 = rhs[2].meta.get<QuaterList>("code");
        }
        code1.set_bool_exit(true_list1, code1.size());
        code.append_quaters(code1);
        code.append_quaters(code2);

        true_list.emplace_back(code1.size());
        false_list = std::move(false_list1);
        for (const auto &item: false_list2) {
            false_list.emplace_back(item + code1.size());
        }

        lhs.meta.set("code", code);
        lhs.meta.set("true_list", true_list);
        lhs.meta.set("false_list", false_list);

    } else if (rhs.size() == 3 && rhs[1].literal == "T_OR") {
        lhs.meta.set("type", BOOL);

        const std::string &value1 = rhs[0].meta.get<std::string>("value");
        const std::string &value2 = rhs[2].meta.get<std::string>("value");
        std::string value = getNewTemp();
        lhs.meta.set("value", value);

        std::vector<size_t> true_list, false_list;
        auto true_list1 = rhs[0].meta.get<std::vector<size_t>>("true_list");
        auto true_list2 = rhs[2].meta.get<std::vector<size_t>>("true_list");
        auto false_list1 = rhs[0].meta.get<std::vector<size_t>>("false_list");
        auto false_list2 = rhs[2].meta.get<std::vector<size_t>>("false_list");

        QuaterList code, code1, code2;
        if (rhs[0].meta.contains("code")) {
            code1 = rhs[0].meta.get<QuaterList>("code");
        }
        if (rhs[2].meta.contains("code")) {
            code2 = rhs[2].meta.get<QuaterList>("code");
        }
        code1.set_bool_exit(false_list1, code1.size());
        code.append_quaters(code1);
        code.append_quaters(code2);

        false_list.emplace_back(code.size() - 1);
        true_list = std::move(true_list1);
        for (const auto &item: true_list2) {
            true_list.emplace_back(item + code1.size());
        }

        lhs.meta.set("code", code);
        lhs.meta.set("true_list", true_list);
        lhs.meta.set("false_list", false_list);
    } else if (rhs.size() == 2) {
        lhs.meta = rhs[1].meta;
        const auto &true_list = rhs[1].meta.get<std::vector<size_t>>("true_list");
        const auto &false_list = rhs[1].meta.get<std::vector<size_t>>("false_list");
        lhs.meta.set("true_list", false_list);
        lhs.meta.set("false_list", true_list);
    }
}

SemanticAnalyzer::SemanticAnalyzer() = default;
