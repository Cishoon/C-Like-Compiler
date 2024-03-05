#include "gtest/gtest.h"
#include "src/MipsInstructionGenerator/RegisterManager.hpp"
#include "src/MipsInstructionGenerator/CodeList.hpp"

size_t Variable::next_id = 1;
TEST(RegisterManagerTest, TestMethod) {
    CodeList code_list;
    RegisterManager manager(2, code_list);
    Variable A("A", VariableType::INT, 1); // 1 - 4
    Variable B("B", VariableType::INT, 1); // 2 - 6
    Variable C("C", VariableType::INT, 1); // 3 - 9
    Variable D("D", VariableType::INT, 1); // 5 - 8
    Variable E("E", VariableType::INT, 1); // 6 - 7

    manager.alloc(A, 1, 4, 1);
    manager.alloc(B, 2, 6, 2);
    manager.alloc(C, 3, 9, 3);
    manager.alloc(D, 5, 8, 5);
    manager.alloc(E, 6, 7, 6);

    SUCCEED();
}

TEST(RegisterManagerTest, TestMethod2) {
    CodeList code_list;
    RegisterManager manager(2, code_list);
    Variable A("A", VariableType::INT, 1); // 1 - 4
    Variable B("B", VariableType::INT, 1); // 2 - 6
    Variable C("C", VariableType::INT, 1); // 3 - 9
    Variable D("D", VariableType::INT, 1); // 5 - 8
    Variable E("E", VariableType::INT, 1); // 6 - 7

    manager.alloc(A, 1, 4, 1);
    manager.alloc(A, 1, 4, 2);
    manager.alloc(A, 1, 4, 3);
    manager.alloc(A, 1, 4, 4);

    code_list.display();

    SUCCEED();
}

TEST(RegisterManagerTest, xx) {
}

