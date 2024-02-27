#pragma once

#include "LR1Parser/Symbol.hpp"
#include "Lexer/Token.hpp"
#include "VariableTable.hpp"
#include <cstdlib>
#include "QuaterList.hpp"


class SemanticAnalyzer {
private:
    VariableTable variable_table;
    size_t variable_deep = 0;

private:
    size_t temp_index = 0;
    std::string getNewTemp();

public:
    SemanticAnalyzer();

    /**
     * 移进的时候，要处理的语法制导
     * @param symbol
     */
    void updateShiftSymbol(Symbol& symbol);

    /**
     * 执行语义动作，
     * @param symbol
     * @param vector1
     */
    void doSemanticAction(Symbol &symbol, const std::vector<Symbol> &vector1);

private:
    void handleVar(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleVarDeclaration(Symbol &lhs, const std::vector<Symbol> &rhs);

    void handleTypeSpecifier(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleSimpleType(Symbol &lhs, const std::vector<Symbol> &rhs);

    void handleExpression(Symbol &lhs, const std::vector<Symbol> &rhs);

    void handleSimpleExpression(Symbol &lhs, const std::vector<Symbol> &rhs);

    void handleAdditiveExpression(Symbol &lhs, const std::vector<Symbol> &rhs);

    void handleTerm(Symbol &lhs, const std::vector<Symbol> &rhs);

    void handleFactor(Symbol &lhs, const std::vector<Symbol> &rhs);

    void handleMulop(Symbol &lhs, const std::vector<Symbol> &rhs);

    void handleAddop(Symbol &lhs, const std::vector<Symbol> &rhs);

    void handleRelop(Symbol &lhs, const std::vector<Symbol> &rhs);

    void handleFunDeclaration(Symbol &lhs, const std::vector<Symbol> &rhs);

    void handleProgram(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleDeclarationList(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleDeclaration(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleExpressionStmt(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleFormalParam(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleParamList(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleParam(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleCompoundStmt(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleStmtList(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleStmt(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleFunCall(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleArgument(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleSelectionStmt(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleReturnStmt(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleIterationStmt(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleOptExpressionStmt(Symbol &symbol, const std::vector<Symbol> &vector);

    void handleBoolExpression(Symbol &symbol, const std::vector<Symbol> &vector);

};


