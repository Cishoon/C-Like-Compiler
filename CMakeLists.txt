cmake_minimum_required(VERSION 3.27)
project(C_Like_Compiler)

set(CMAKE_CXX_STANDARD 17)

include_directories(src include)

#if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#    add_compile_options(-O2)
#elseif(MSVC)
#    add_compile_options(/O2)
#else()
#    message(STATUS "Unknown compiler, optimization flag -O2 might not be set properly.")
#endif()

add_executable(C_Like_Compiler
        src/Lexer/Lexer.cpp
        include/src/Lexer/Lexer.hpp
        src/LR1Parser/LR1Parser.cpp
        include/src/LR1Parser/LR1Parser.hpp
        src/main.cpp
        include/src/Quater/Quater.hpp
        include/src/Lexer/Token.hpp
        src/Compiler/Compiler.cpp
        include/src/Compiler/Compiler.hpp
        src/Semantic/SemanticAnalyzer.cpp
        include/src/Semantic/SemanticAnalyzer.hpp
        include/src/LR1Parser/LR1Item.hpp
        include/src/LR1Parser/Symbol.hpp
        include/src/LR1Parser/Production.hpp
        src/common/MetaData.cpp
        include/src/common/MetaData.hpp
        include/src/Variable/VariableTable.hpp
        include/src/Quater/QuaterList.hpp
        include/src/common/dlog.hpp
        src/MipsInstructionGenerator/MipsInstructionGenerator.cpp
        include/src/MipsInstructionGenerator/MipsInstructionGenerator.hpp
        include/src/common/utils.hpp
        include/src/MipsInstructionGenerator/RegisterManager.hpp
        include/src/Variable/Variable.hpp
        include/src/Variable/VariableType.hpp
        include/src/MipsInstructionGenerator/Rvalue.hpp
        include/src/MipsInstructionGenerator/Avalue.hpp
        src/MipsInstructionGenerator/Variable.cpp
        include/src/MipsInstructionGenerator/CodeList.hpp
        src/MipsInstructionGenerator/RegisterManager.cpp
        include/src/Function/FunctionTable.hpp
        include/src/Function/Function.hpp
        include/src/Lexer/TokenType.hpp
        include/src/LR1Parser/Action.hpp
        src/MipsInstructionGenerator/StackManager.cpp
        include/src/MipsInstructionGenerator/StackManager.hpp
)

# Google Test集成
# 将Google Test添加为一个子目录
add_subdirectory(external/googletest)

# 包含Google Test的头文件
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# 定义测试可执行文件
add_executable(Tests
        tests/test_main.cpp
        tests/test_RegisterManager.cpp
        # 添加你的测试文件
)

# 链接Google Test库和你的项目文件到测试可执行文件
target_link_libraries(Tests gtest gtest_main)

enable_testing()
add_test(NAME AllTests COMMAND Tests)