#include "compiler.h"

#include <iostream>

#include "codegen.hpp"
#include "lexer.h"
#include "parser.h"

int main(int argc, char* argv[]) {
    std::string CodeStr;
    for (int i = 1; i < argc; i++) {
        if (!CodeStr.empty()) CodeStr += ' ';
        CodeStr += argv[i];
    }
    CodeStr = "var x = 12 - 4 x = x + 5";

    std::vector<sprout::lexer::Token> tokens = sprout::lexer::tokenize(CodeStr);
    sprout::memManager::Memory mem {};
    sprout::parser::ASTNode* tree = sprout::parser::parseProgram(tokens, mem);
    sprout::parser::printAST(tree, 1);
    sprout::codegen::BCData data {};
    sprout::codegen::compileProgram(data, static_cast<sprout::parser::ProgramNode*>(tree));
    sprout::codegen::disassemble(data);
    sprout::memManager::freeMemory(mem);
    return 0;
}