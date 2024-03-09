#include "src/Semantic/SemanticAnalyzer.hpp"
#include "src/common/dlog.hpp"
#include "src/MipsInstructionGenerator/MipsInstructionGenerator.hpp"


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
    } else if (lhs.literal == "<fun_definition>") {
        handleFunDefinition(lhs, rhs);
    } else if (lhs.literal == "<type_specifier>") {
        handleTypeSpecifier(lhs, rhs);
    } else if (lhs.literal == "<expression_stmt>") {
        handleExpressionStmt(lhs, rhs);
    } else if (lhs.literal == "<simple_type>") {
        handleSimpleType(lhs, rhs);
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

        QuaterList quat;
        if (rhs[3].meta.contains("quat")) {
            quat.append_quaters(rhs[3].meta.get<QuaterList>("quat"));
        }
        quat.add_quater("=", value2, "", name, variable_table);
        lhs.meta.set("quat", quat);
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
        if (rhs[0].meta.contains("quat")) {
            lhs.meta.set("quat", rhs[0].meta.get<QuaterList>("quat"));
        }
    } else if (rhs.size() == 3 && rhs[2].literal == "<expression>") {
        const std::string &name = rhs[0].meta.get<std::string>("name");

        auto item = variable_table.lookup(name);

        if (!item.has_value()) {
            std::cerr << "未定义变量：" << name << std::endl;
            // exit(6);
            throw std::exception();
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
        QuaterList quat;
        if (rhs[0].meta.contains("quat")) {
            const auto &quat1 = rhs[0].meta.get<QuaterList>("quat");
            quat.append_quaters(quat1);
        }
        if (rhs[2].meta.contains("quat")) {
            const auto &quat2 = rhs[2].meta.get<QuaterList>("quat");
            quat.append_quaters(quat2);
        }

        quat.add_quater(op, value2, "", name, variable_table);
        lhs.meta.set("quat", quat);
    } else if (rhs.size() == 3 && rhs[2].literal == "<bool_expression>") {
        const std::string &name = rhs[0].meta.get<std::string>("name");

        auto item = variable_table.lookup(name);
        if (!item.has_value()) {
            std::cerr << "未定义变量：" << name << std::endl;
            // exit(6);
            throw std::exception();
        }
        const VariableType type1 = item.value().type;

        const VariableType &type2 = rhs[2].meta.get<VariableType>("type");
        if (type2 > type1) {
            std::cout << "发生截断" << std::endl;
        }
        lhs.meta.set("type", type1);

        const auto &value2 = getNewTemp();
        lhs.meta.set("value", name);

        const std::string &op = rhs[1].real_value;
        QuaterList quat;
        if (rhs[0].meta.contains("quat")) {
            const auto &quat1 = rhs[0].meta.get<QuaterList>("quat");
            quat.append_quaters(quat1);
        }
        if (rhs[2].meta.contains("quat")) {
            auto quat2 = rhs[2].meta.get<QuaterList>("quat");
            const auto &true_list = rhs[2].meta.get<std::vector<size_t>>("true_list");
            const auto &false_list = rhs[2].meta.get<std::vector<size_t>>("false_list");

            const auto &temp = value2;
            const auto &true_exit = quat2.add_quater("=", "1", "", temp, variable_table);
            quat2.add_quater("j", "", "", std::to_string(true_exit + 3), variable_table);
            const auto &false_exit = quat2.add_quater("=", "0", "", temp, variable_table);

            quat2.set_bool_exit(true_list, true_exit);
            quat2.set_bool_exit(false_list, false_exit);
            quat.append_quaters(quat2);
        }

        quat.add_quater(op, value2, "", name, variable_table);
        lhs.meta.set("quat", quat);
    }
}

void SemanticAnalyzer::handleSimpleExpression(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <additive_expression> | <bool_expression>
    if (rhs.size() == 1) {
        lhs.meta.set("type", rhs[0].meta.get<VariableType>("type"));
        lhs.meta.set("value", rhs[0].meta.get<std::string>("value"));
        if (!rhs[0].meta.contains("quat")) return;

        auto quat = rhs[0].meta.get<QuaterList>("quat");

        if (rhs[0].literal == "<bool_expression>") {
            const auto &true_list = rhs[0].meta.get<std::vector<size_t>>("true_list");
            const auto &false_list = rhs[0].meta.get<std::vector<size_t>>("false_list");

            const auto &temp = rhs[0].meta.get<std::string>("value");
            const auto &true_exit = quat.add_quater("=", "1", "", temp, variable_table);
            quat.add_quater("j", "", "", std::to_string(true_exit + 3), variable_table);
            const auto &false_exit = quat.add_quater("=", "0", "", temp, variable_table);

            quat.set_bool_exit(true_list, true_exit);
            quat.set_bool_exit(false_list, false_exit);
        }

        lhs.meta.set("quat", quat);
    }
}

void SemanticAnalyzer::handleAdditiveExpression(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <term> | <additive_expression> <addop> <term>
    if (rhs.size() == 1) {
        lhs.meta.set("type", rhs[0].meta.get<VariableType>("type"));
        lhs.meta.set("value", rhs[0].meta.get<std::string>("value"));
        if (rhs[0].meta.contains("quat")) {
            lhs.meta.set("quat", rhs[0].meta.get<QuaterList>("quat"));
        }
    } else if (rhs.size() == 3) {
        const std::string &op = rhs[1].meta.get<std::string>("op");
        const VariableType &type1 = rhs[0].meta.get<VariableType>("type");
        const VariableType &type2 = rhs[2].meta.get<VariableType>("type");
        VariableType type = std::max(type1, type2);
        lhs.meta.set("type", type);

        const std::string &value1 = rhs[0].meta.get<std::string>("value");
        const std::string &value2 = rhs[2].meta.get<std::string>("value");
        auto value = getNewTemp();
        lhs.meta.set("value", value);

        QuaterList quat;
        if (rhs[0].meta.contains("quat")) {
            const auto &quat1 = rhs[0].meta.get<QuaterList>("quat");
            quat.append_quaters(quat1);
        }
        if (rhs[2].meta.contains("quat")) {
            const auto &quat2 = rhs[2].meta.get<QuaterList>("quat");
            quat.append_quaters(quat2);
        }

        quat.add_quater(op, value1, value2, value, variable_table);
        lhs.meta.set("quat", quat);
    }
}

void SemanticAnalyzer::handleTerm(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <factor> | <term> <mulop> <factor>
    if (rhs.size() == 1) {
        lhs.meta.set("type", rhs[0].meta.get<VariableType>("type"));
        lhs.meta.set("value", rhs[0].meta.get<std::string>("value"));
        if (rhs[0].meta.contains("quat")) {
            lhs.meta.set("quat", rhs[0].meta.get<QuaterList>("quat"));
        }
    } else {
        const std::string &op = rhs[1].meta.get<std::string>("op");
        const VariableType &type1 = rhs[0].meta.get<VariableType>("type");
        const VariableType &type2 = rhs[2].meta.get<VariableType>("type");
        VariableType type = std::max(type1, type2);
        lhs.meta.set("type", type);

        const std::string &value1 = rhs[0].meta.get<std::string>("value");
        const std::string &value2 = rhs[2].meta.get<std::string>("value");
        auto value = getNewTemp();
        lhs.meta.set("value", value);

        QuaterList quat;
        if (rhs[0].meta.contains("quat")) {
            const auto &quat1 = rhs[0].meta.get<QuaterList>("quat");
            quat.append_quaters(quat1);
        }
        if (rhs[2].meta.contains("quat")) {
            const auto &quat2 = rhs[2].meta.get<QuaterList>("quat");
            quat.append_quaters(quat2);
        }

        quat.add_quater(op, value1, value2, value, variable_table);

        lhs.meta.set("quat", quat);
    }
}

void SemanticAnalyzer::handleFactor(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // T_LEFT_PAREN <expression> T_RIGHT_PAREN | <var> | <fun_call> | T_INTEGER_LITERAL | T_STRING_LITERAL | T_CHAR_LITERAL | T_FLOAT_LITERAL
    if (rhs.size() == 3) {
        lhs.meta.set("type", rhs[1].meta.get<VariableType>("type"));
        lhs.meta.set("value", rhs[1].meta.get<std::string>("value"));
        if (rhs[1].meta.contains("quat")) {
            lhs.meta.set("quat", rhs[1].meta.get<QuaterList>("quat"));
        }
    } else if (rhs.size() == 1) {
        const std::string &_literal = rhs[0].literal;
        if (_literal == "<var>") {
            auto name = rhs[0].meta.get<std::string>("name");
            auto item = variable_table.lookup(name);
            if (!item.has_value()) {
                std::cerr << "未定义变量：" << name << std::endl;
                // exit(6);
                throw std::exception();
            }
            lhs.meta.set("type", item.value().type);
            lhs.meta.set("value", name);
        } else if (_literal == "<fun_call>") {
            auto type = rhs[0].meta.get<VariableType>("type");
            lhs.meta.set("type", type);

            if (type == VariableType::VOID) {
                static std::string value = "void";
                lhs.meta.set("value", value);
            } else {
                auto value = rhs[0].meta.get<std::string>("value");
                lhs.meta.set("value", value);
            };

            auto quat = rhs[0].meta.get<QuaterList>("quat");
            lhs.meta.set("quat", quat);

        } else if (_literal == "T_INTEGER_LITERAL") {
            lhs.meta.set("type", VariableType::INT);
            lhs.meta.set("value", rhs[0].real_value);
        } else {
            std::cerr << "暂时不支持处理 factor.literal = " << _literal << std::endl;
            // exit(-1);
            throw std::exception();
        }
    }

}


std::string SemanticAnalyzer::getNewTemp() {
    // 默认类型全部是INT
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
    // <fun_declaration> ::= <fun_definition> <compound_stmt>
    QuaterList quat;
    if (rhs[0].meta.contains("quat")) {
        quat.append_quaters(rhs[0].meta.get<QuaterList>("quat"));
    }
    if (rhs[1].meta.contains("quat")) {
        quat.append_quaters(rhs[1].meta.get<QuaterList>("quat"));
    }
    auto name = rhs[0].meta.get<std::string>("name");
    if (show_output) {
        std::cout << name << std::endl;
        quat.display();
    }

    auto mips_generator = MipsInstructionGenerator(quat, name);
    mips_generator.generator();
    lhs.meta.set("code", mips_generator.get_code_list());
}

void SemanticAnalyzer::handleProgram(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <Program> ::= <declaration_list>
    finalMips = MipsInstructionGenerator::generate_header();
    auto mips_list = std::move(rhs[0].meta.get<CodeList>("code"));
    finalMips.append(mips_list);
}

void SemanticAnalyzer::handleDeclarationList(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <declaration_list> ::= <declaration> <declaration_list> | <declaration>
    // using DeclarationList = std::vector<std::vector<Symbol>>;
    if (rhs.size() == 1) {
        auto mips = rhs[0].meta.get<CodeList>("code");
        lhs.meta.set("code", mips);
    } else if (rhs.size() == 2) {
        auto mips_list = rhs[1].meta.get<CodeList>("code");
        auto mips = rhs[0].meta.get<CodeList>("code");
        mips_list.code_list.insert(mips_list.code_list.end(), mips.code_list.begin(), mips.code_list.end());
        lhs.meta.set("code", mips_list);
    }
}

void SemanticAnalyzer::handleDeclaration(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <declaration> ::= <var_declaration> | <fun_declaration>
    if (rhs[0].literal == "<fun_declaration>") {
        const auto &code = rhs[0].meta.get<CodeList>("code");
        lhs.meta.set("code", code);
    } else if (rhs[0].literal == "<var_declaration>") {
        return;
    }
}

void SemanticAnalyzer::handleExpressionStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <expression_stmt> ::= <expression> T_SEMICOLON
    if (rhs[0].meta.contains("quat")) {
        const auto &quat = rhs[0].meta.get<QuaterList>("quat");
        lhs.meta.set("quat", quat);
    }
}

void SemanticAnalyzer::handleParamList(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <param_list> ::= <param> | <param_list> T_COMMA <param>
    if (rhs.size() == 1) {
        auto name = rhs[0].meta.get<std::string>("name");
        auto type = rhs[0].meta.get<VariableType>("type");
        std::vector<Variable> param_list;
        param_list.emplace_back(name, type, 1);
        lhs.meta.set("param_list", param_list);
        variable_table.add_item(name, type, 1, (int) param_list.size() - 1);
    } else {
        auto name = rhs[2].meta.get<std::string>("name");
        auto type = rhs[2].meta.get<VariableType>("type");
        auto param_list = rhs[0].meta.get<std::vector<Variable>>("param_list");
        param_list.emplace_back(name, type, 1);
        lhs.meta.set("param_list", param_list);
        variable_table.add_item(name, type, 1, (int) param_list.size() - 1);
    }
}

void SemanticAnalyzer::handleParam(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <param> ::= <type_specifier> T_IDENTIFIER | <type_specifier> T_IDENTIFIER T_LEFT_SQUARE T_RIGHT_SQUARE
    if (rhs.size() == 2) {
        auto name = rhs[1].real_value;
        auto type = rhs[0].meta.get<VariableType>("type");
        // variable_table.add_item(name, type, 1);
        lhs.meta.set("name", name);
        lhs.meta.set("type", type);
    } else {
        std::cerr << "函数调用暂时不支持数组" << std::endl;
        // exit(-1);
        throw std::exception();
    }
}

void SemanticAnalyzer::handleCompoundStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <compound_stmt> ::= T_LEFT_BRACE <stmt_list> T_RIGHT_BRACE | T_LEFT_BRACE T_RIGHT_BRACE
    if (rhs.size() == 2) return;
    if (rhs[1].meta.contains("quat")) {
        const auto &quat = rhs[1].meta.get<QuaterList>("quat");
        lhs.meta.set("quat", quat);
    }
}

void SemanticAnalyzer::handleStmtList(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <stmt_list> ::= <stmt> <stmt_list> | <stmt>
    if (rhs.size() == 1) {
        if (rhs[0].meta.contains("quat")) {
            const auto &quat = rhs[0].meta.get<QuaterList>("quat");
            lhs.meta.set("quat", quat);
        }
    } else if (rhs.size() == 2) {
        QuaterList quat;
        if (rhs[0].meta.contains("quat")) {
            const auto &quat1 = rhs[0].meta.get<QuaterList>("quat");
            quat.append_quaters(quat1);
        }
        if (rhs[1].meta.contains("quat")) {
            const auto &quat2 = rhs[1].meta.get<QuaterList>("quat");
            quat.append_quaters(quat2);
        }
        lhs.meta.set("quat", quat);
    }


}

void SemanticAnalyzer::handleStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <stmt> ::= <var_declaration> | <expression_stmt> | <compound_stmt> | <selection_stmt> | <iteration_stmt> | <return_stmt>
    if (rhs[0].meta.contains("quat")) {
        const auto &quat = rhs[0].meta.get<QuaterList>("quat");
        lhs.meta.set("quat", quat);
    }
}

void SemanticAnalyzer::handleFunCall(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <fun_call> ::=
    // T_IDENTIFIER T_LEFT_PAREN <argument> T_RIGHT_PAREN |
    // T_IDENTIFIER T_LEFT_PAREN T_RIGHT_PAREN

    std::string fun_name = rhs[0].real_value;

    auto func = function_table.lookup(fun_name);
    if (!func.has_value()) {
        std::cerr << "未定义函数: " << fun_name << std::endl;
        // exit(-9);
        throw std::exception();
    }

    QuaterList quat;

    size_t args_num = 0;
    std::vector<Variable> args;
    // 有变量，将变量传值
    if (rhs.size() == 4) {
        args = rhs[2].meta.get<std::vector<Variable>>("args");
        args_num = args.size();
        if (rhs[2].meta.contains("quat")) {
            quat.append_quaters(rhs[2].meta.get<QuaterList>("quat"));
        }
        for (size_t i = 0; i < args.size(); i++) {
            const auto &item = args[i];
            // quat.add_quater("=", item.name, "", "param" + std::to_string(i), variable_table);
            quat.add_quater("param", item.name, std::to_string(args.size()), std::to_string(i), variable_table);
        }
    }

    if (args_num != func.value().formal_variables.size()) {
        std::cerr << "函数参数数量不匹配" << std::endl;
        // exit(11);
        throw std::exception();
    }

    // 有无返回值
    if (func.value().return_type == VariableType::VOID) {
        auto _id = quat.add_quater("call", fun_name, std::to_string(args_num), "", variable_table);
        quat[_id].second.meta.set("args", args);
        lhs.meta.set("value", "");
        lhs.meta.set("type", func.value().return_type);
    } else {
        std::string value = getNewTemp();
        auto _id = quat.add_quater("call", fun_name, std::to_string(args_num), value, variable_table);
        quat[_id].second.meta.set("args", args);
        lhs.meta.set("value", value);
        lhs.meta.set("type", func.value().return_type);
    }

    lhs.meta.set("quat", quat);
}

void SemanticAnalyzer::handleArgument(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <argument> ::= <expression> | <argument> T_COMMA <expression>
    if (rhs.size() == 1) {
        std::vector<Variable> args;
        auto value = rhs[0].meta.get<std::string>("value");
        auto type = rhs[0].meta.get<VariableType>("type");
        args.emplace_back(value, type, -1);

        lhs.meta.set("args", args);
        if (rhs[0].meta.contains("quat")) {
            const auto &quat = rhs[0].meta.get<QuaterList>("quat");
            lhs.meta.set("quat", quat);
        }
    } else {
        auto args = rhs[0].meta.get<std::vector<Variable>>("args");

        auto value = rhs[2].meta.get<std::string>("value");
        auto type = rhs[2].meta.get<VariableType>("type");
        auto var = variable_table.lookup(value);
        if (var.has_value()) {
            args.emplace_back(var.value());
        } else {
            args.emplace_back(value, type, -1);
        }

        lhs.meta.set("args", args);

        QuaterList quat;

        if (rhs[0].meta.contains("quat")) {
            const auto &quat1 = rhs[0].meta.get<QuaterList>("quat");
            quat.append_quaters(quat1);
        }
        if (rhs[2].meta.contains("quat")) {
            const auto &quat2 = rhs[2].meta.get<QuaterList>("quat");
            quat.append_quaters(quat2);
        }

        lhs.meta.set("quat", quat);
    }
}

void SemanticAnalyzer::handleSelectionStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // T_IF T_LEFT_PAREN <bool_expression> T_RIGHT_PAREN <stmt> |
    // T_IF T_LEFT_PAREN <bool_expression> T_RIGHT_PAREN <stmt> T_ELSE <stmt>

    if (rhs.size() == 5) {
        auto quat = rhs[2].meta.get<QuaterList>("quat");
        const auto &true_list = rhs[2].meta.get<std::vector<size_t >>("true_list");
        const auto &false_list = rhs[2].meta.get<std::vector<size_t >>("false_list");

        const auto &quat2 = rhs[4].meta.get<QuaterList>("quat");

        const auto &true_exit = quat.append_quaters(quat2);
        quat.set_bool_exit(true_list, true_exit);
        quat.set_bool_exit(false_list, quat.size());

        lhs.meta.set("quat", quat);
    } else if (rhs.size() == 7) {
        auto quat = rhs[2].meta.get<QuaterList>("quat");
        const auto &true_list = rhs[2].meta.get<std::vector<size_t >>("true_list");
        const auto &false_list = rhs[2].meta.get<std::vector<size_t >>("false_list");

        const auto &quat2 = rhs[4].meta.get<QuaterList>("quat");
        const auto &quat3 = rhs[6].meta.get<QuaterList>("quat");

        const auto &true_exit = quat.append_quaters(quat2);
        quat.add_quater("j", "", "", std::to_string(quat.size() + quat3.size() + 1), variable_table);
        const auto &false_exit = quat.append_quaters(quat3);

        quat.set_bool_exit(true_list, true_exit);
        quat.set_bool_exit(false_list, false_exit);

        lhs.meta.set("quat", quat);
    }
}

void SemanticAnalyzer::handleReturnStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <return_stmt> ::= T_RETURN T_SEMICOLON | T_RETURN <expression> T_SEMICOLON
    if (rhs.size() == 2) {
        QuaterList quat;
        quat.add_quater("return", "", "", "", variable_table);
        lhs.meta.set("quat", quat);
    } else if (rhs.size() == 3) {
        QuaterList quat;
        if (rhs[1].meta.contains("quat")) {
            quat.append_quaters(rhs[1].meta.get<QuaterList>("quat"));
        }
        auto var = rhs[1].meta.get<std::string>("value");
        quat.add_quater("return", var, "", "", variable_table);
        lhs.meta.set("quat", quat);
    }
}

void SemanticAnalyzer::handleIterationStmt(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // T_WHILE T_LEFT_PAREN <bool_expression> T_RIGHT_PAREN <stmt> |
    if (rhs.size() == 5) {
        auto quat = rhs[2].meta.get<QuaterList>("quat");
        const auto &quat2 = rhs[4].meta.get<QuaterList>("quat");
        const auto &true_list = rhs[2].meta.get<std::vector<size_t >>("true_list");
        const auto &false_list = rhs[2].meta.get<std::vector<size_t >>("false_list");

        const auto &true_exit = quat.append_quaters(quat2);
        quat.add_quater("j", "", "", "0", variable_table);

        quat.set_bool_exit(true_list, true_exit);
        quat.set_bool_exit(false_list, quat.size());

        lhs.meta.set("quat", quat);
    } else if (rhs.size() == 8) {
        // T_FOR T_LEFT_PAREN <可选表达式语句> <布尔表达式> T_SEMICOLON <可选表达式语句> T_RIGHT_PAREN <语句>
        const auto &quat1 = rhs[2].meta.get<QuaterList>("quat");
        const auto &quat2 = rhs[3].meta.get<QuaterList>("quat");
        const auto &code3 = rhs[5].meta.get<QuaterList>("quat");
        const auto &code4 = rhs[7].meta.get<QuaterList>("quat");
        const auto &true_list = rhs[3].meta.get<std::vector<size_t>>("true_list");
        const auto &false_list = rhs[3].meta.get<std::vector<size_t>>("false_list");


        auto quat = quat1;
        const auto &loop = quat.append_quaters(quat2);
        const auto &true_exit = quat.append_quaters(code4);
        quat.append_quaters(code3);
        quat.add_quater("j", "", "", std::to_string(loop), variable_table);
        quat.set_bool_exit(true_list, true_exit, loop);
        quat.set_bool_exit(false_list, quat.size(), loop);

        lhs.meta.set("quat", quat);
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

        QuaterList quat;
        if (rhs[0].meta.contains("quat")) {
            const auto &quat1 = rhs[0].meta.get<QuaterList>("quat");
            quat.append_quaters(quat1);
        }
        if (rhs[2].meta.contains("quat")) {
            const auto &quat2 = rhs[2].meta.get<QuaterList>("quat");
            quat.append_quaters(quat2);
        }

        std::vector<size_t> true_list, false_list;
        auto true_id = quat.add_quater("j" + op, value1, value2, "", variable_table);
        auto false_id = quat.add_quater("j", "", "", "", variable_table);
        true_list.emplace_back(true_id);
        false_list.emplace_back(false_id);

        lhs.meta.set("quat", quat);
        lhs.meta.set("true_list", true_list);
        lhs.meta.set("false_list", false_list);
    } else if (rhs.size() == 3 && rhs[1].literal == "<bool_expression>") {
        lhs.meta = rhs[1].meta;
    } else if (rhs.size() == 3 && rhs[1].literal == "T_AND") {
        lhs.meta.set("type", BOOL);

        // const std::string &value1 = rhs[0].meta.get<std::string>("value");
        // const std::string &value2 = rhs[2].meta.get<std::string>("value");
        auto value = getNewTemp();
        lhs.meta.set("value", value);

        std::vector<size_t> true_list, false_list;
        auto true_list1 = rhs[0].meta.get<std::vector<size_t>>("true_list");
        auto true_list2 = rhs[2].meta.get<std::vector<size_t>>("true_list");
        auto false_list1 = rhs[0].meta.get<std::vector<size_t>>("false_list");
        auto false_list2 = rhs[2].meta.get<std::vector<size_t>>("false_list");

        QuaterList quat, quat1, quat2;
        if (rhs[0].meta.contains("quat")) {
            quat1 = rhs[0].meta.get<QuaterList>("quat");
        }
        if (rhs[2].meta.contains("quat")) {
            quat2 = rhs[2].meta.get<QuaterList>("quat");
        }
        quat1.set_bool_exit(true_list1, quat1.size());
        quat.append_quaters(quat1);
        quat.append_quaters(quat2);

        true_list.emplace_back(quat1.size());
        false_list = std::move(false_list1);
        for (const auto &item: false_list2) {
            false_list.emplace_back(item + quat1.size());
        }

        lhs.meta.set("quat", quat);
        lhs.meta.set("true_list", true_list);
        lhs.meta.set("false_list", false_list);

    } else if (rhs.size() == 3 && rhs[1].literal == "T_OR") {
        lhs.meta.set("type", BOOL);

        // const std::string &value1 = rhs[0].meta.get<std::string>("value");
        // const std::string &value2 = rhs[2].meta.get<std::string>("value");
        auto value = getNewTemp();
        lhs.meta.set("value", value);

        std::vector<size_t> true_list, false_list;
        auto true_list1 = rhs[0].meta.get<std::vector<size_t>>("true_list");
        auto true_list2 = rhs[2].meta.get<std::vector<size_t>>("true_list");
        auto false_list1 = rhs[0].meta.get<std::vector<size_t>>("false_list");
        auto false_list2 = rhs[2].meta.get<std::vector<size_t>>("false_list");

        QuaterList quat, quat1, quat2;
        if (rhs[0].meta.contains("quat")) {
            quat1 = rhs[0].meta.get<QuaterList>("quat");
        }
        if (rhs[2].meta.contains("quat")) {
            quat2 = rhs[2].meta.get<QuaterList>("quat");
        }
        quat1.set_bool_exit(false_list1, quat1.size());
        quat.append_quaters(quat1);
        quat.append_quaters(quat2);

        false_list.emplace_back(quat.size() - 1);
        true_list = std::move(true_list1);
        for (const auto &item: true_list2) {
            true_list.emplace_back(item + quat1.size());
        }

        lhs.meta.set("quat", quat);
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

SemanticAnalyzer::SemanticAnalyzer(bool show_output) : show_output(show_output) {
    Function read(VariableType::INT, "read", {});
    function_table.add_func(read);
    Function write(VariableType::VOID, "write", {Variable("a", VariableType::INT, 0)});
    function_table.add_func(write);
}

void SemanticAnalyzer::handleFunDefinition(Symbol &lhs, const std::vector<Symbol> &rhs) {
    // <fun_definition> ::=
    // <type_specifier> T_IDENTIFIER T_LEFT_PAREN <param_list> T_RIGHT_PAREN |
    // <type_specifier> T_IDENTIFIER T_LEFT_PAREN T_RIGHT_PAREN
    if (rhs.size() == 4) {
        //  <type_specifier> T_IDENTIFIER T_LEFT_PAREN T_RIGHT_PAREN
        Function function(rhs[0].meta.get<VariableType>("type"), rhs[1].real_value, {});
        function_table.add_func(function);

        lhs.meta.set("name", rhs[1].real_value);
        lhs.meta.set("ret_type", rhs[0].meta.get<VariableType>("type"));
    } else {
        //  <type_specifier> T_IDENTIFIER T_LEFT_PAREN <param_list> T_RIGHT_PAREN |
        QuaterList quat;

        const auto &param_list = rhs[3].meta.get<std::vector<Variable>>("param_list");
        for (auto i = 0; i < param_list.size(); i++) {
            const auto &param = "param" + std::to_string(i);
            variable_table.add_item(param, param_list[i].type, 1, i);
            quat.add_quater("=", param, "", param_list[i].name, variable_table);
        }

        Function function(rhs[0].meta.get<VariableType>("type"), rhs[1].real_value, param_list);
        function_table.add_func(function);

        lhs.meta.set("quat", quat);
        lhs.meta.set("name", rhs[1].real_value);
        lhs.meta.set("ret_type", rhs[0].meta.get<VariableType>("type"));
    }
}
