#pragma once

#include <string>
#include <set>
#include <utility>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include "Semantic/Quater.hpp"
#include "Production.hpp"
#include "Symbol.hpp"
#include "LR1Item.hpp"


struct Action {
    enum class Type {
        SHIFT,
        REDUCE,
        ACCEPT,
        ERROR
    };
    Type type;
    size_t number;          // shift的新状态编号
    Production production;  // reduce使用的产生式
    Action() { type = Type::ERROR, number = -1, production = Production(); };

    Action(const Type &type, const size_t &number, Production production) : type(type), number(number),
                                                                            production(std::move(production)) {}

    [[nodiscard]] std::string to_string() const {
        switch (type) {
            case Type::SHIFT:
                return "S " + std::to_string(number);
            case Type::REDUCE:
                return "R " + production.to_string();
            case Type::ACCEPT:
                return "Accept";
            case Type::ERROR:
                return "x";
        }
        return "";
    }


    friend std::ostream &operator<<(std::ostream &os, Action::Type &_type) {
        return os << static_cast<int>(_type);
    }

    friend std::istream &operator>>(std::istream &is, Action::Type &_type) {
        int intType;
        is >> intType;
        _type = static_cast<Type>(intType);
        return is;
    }


    friend std::ostream &operator<<(std::ostream &os, Action &action) {
        return os << action.type << " " << action.number << " " << action.production;
    }

    friend std::istream &operator>>(std::istream &is, Action &action) {
        int temp;
        is >> temp >> action.number >> action.production;
        action.type = static_cast<Type>(temp);
        return is;
    }
};




class LR1Parser {
public:
    LR1Parser(const std::vector<Production> &productions, Symbol start, Symbol end);

    explicit LR1Parser(const std::string &file_path);

    LR1Parser() = default;

    void print_firstSet() const;

    void print_tables() const;

    bool parse(const std::vector<Symbol> &sentence);

    Action get_next_action(const Symbol &currentSymbol);

    void save_tables(const std::string &file_path);

    void load_tables(const std::string &file_path);

private:
    void
    parse_EBNF_line(const std::string &line);

    static void print_stacks(const std::stack<int> &stateStack,
                             const std::stack<Symbol> &symbolStack,
                             const std::vector<Symbol> &inputStack);

    void construct_tables();

    /**
     * @brief 求项目集族的闭包
     *
     * @param lr1ItemSet 待求闭包的项目集族
     * @return std::unordered_set<LR1Item, LR1ItemHash, LR1ItemEqual> 该项目集族的闭包
     */
    void closure(std::unordered_set<LR1Item, LR1ItemHash, LR1ItemEqual> &lr1ItemSet) const;

private:
    /**
     * @brief 获取一个LR1项目当前的状态(移进、归约、接受、待约)
     *
     * @param item LR1项目
     * @return LR1Item::State 状态
     */
    [[nodiscard]] LR1Item::State get_lr1item_state(const LR1Item &item) const;

    /**
     * @brief 获取由某非终结符为产生式左边的所有产生式
     *
     * @param symbol 非终结符
     * @return std::vector<Production> 所有产生式
     */
    [[nodiscard]] std::vector<Production> get_productions_start_by_symbol(const Symbol &symbol) const;

private:  // 求FIRST集
    void calculate_firstSets();

    [[nodiscard]] std::unordered_set<Symbol, SymbolHash, SymbolEqual>
    get_first_string(const std::vector<Symbol> &content) const;

private:
    std::vector<Production> productions;
    std::unordered_map<Symbol, std::vector<Production>, SymbolHash, SymbolEqual> productionMap;
    std::unordered_map<Symbol, std::unordered_set<Symbol, SymbolHash, SymbolEqual>, SymbolHash, SymbolEqual> firstSet, followSet;

    Symbol start_symbol;  // 起始符
    Symbol end_symbol;    // 终止符

    std::vector<std::unordered_set<LR1Item, LR1ItemHash, LR1ItemEqual>> lr1ItemSets;  // 项目集族


    std::unordered_set<std::string> terminals;  // 终结符集

public:
    std::stack<size_t> stateStack;                                          // 状态栈
    std::stack<Symbol> symbolStack;                                                     // 符号栈
    std::map<std::pair<int, Symbol>, int> gotoTable;                                  // GOTO表
    std::map<std::pair<int, Symbol>, Action> actionTable;                             // ACTION表

};
