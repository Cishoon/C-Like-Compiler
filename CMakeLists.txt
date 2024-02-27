cmake_minimum_required(VERSION 3.27)
project(C_Like_Compiler)

set(CMAKE_CXX_STANDARD 17)

include_directories(src include)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-O2)
elseif(MSVC)
    add_compile_options(/O2)
else()
    message(STATUS "Unknown compiler, optimization flag -O2 might not be set properly.")
endif()

add_executable(C_Like_Compiler
        src/Lexer/Lexer.cpp
        src/Lexer/Lexer.hpp
        src/LR1Parser/LR1Parser.cpp
        src/LR1Parser/LR1Parser.hpp
        src/main.cpp
        src/Semantic/Quater.hpp
        src/Lexer/Token.hpp
        src/Compiler.cpp
        src/Compiler.hpp
        src/Semantic/SemanticAnalyzer.cpp
        src/Semantic/SemanticAnalyzer.hpp
        src/LR1Parser/LR1Item.hpp
        src/LR1Parser/Symbol.hpp
        src/LR1Parser/Production.hpp
        src/common/MetaData.cpp
        src/common/MetaData.hpp
        src/Semantic/VariableTable.cpp
        src/Semantic/VariableTable.hpp
        src/Semantic/QuaterList.hpp
        src/common/dlog.hpp
        src/MipsInstructionGenerator/MipsInstructionGenerator.cpp
        src/MipsInstructionGenerator/MipsInstructionGenerator.hpp
        src/MipsInstructionGenerator/MipsInstruction.hpp
        src/common/utils.hpp
        src/MipsInstructionGenerator/TempRegManager.hpp)