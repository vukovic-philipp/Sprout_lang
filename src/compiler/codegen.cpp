#include "codegen.hpp"
#include "values.h"

namespace sprout::codegen {
    uint8_t compileExpr(BCData& b, parser::ASTNode* node) {
        switch (node->type) {
            case parser::NODE_INT_LITERAL: {
                auto* n = static_cast<parser::IntLiteralNode*>(node);
                uint8_t reg = allocGP(b);
                emitLoad64(b, reg, encodeInt(n->number));
                return reg;
            }
            case parser::NODE_BINARY_OP: {
                auto* n = static_cast<parser::BinaryOpNode*>(node);
                uint8_t result = allocGP(b);
                uint8_t left = compileExpr(b, n->left);
                uint8_t right = compileExpr(b, n->right);

                if (n->op == lexer::PLUS) emitInstr(b, OP_ADD, result, left, right);
                if (n->op == lexer::MINUS) emitInstr(b, OP_SUB, result, left, right);
                if (n->op == lexer::STAR) emitInstr(b, OP_MUL, result, left, right);
                if (n->op == lexer::SLASH) emitInstr(b, OP_DIV, result, left, right);

                deallocGP(b, right);
                deallocGP(b, left);
                return result;
            }
            case parser::NODE_IDENT: {
                auto* n = static_cast<parser::IdentNode*>(node);
                uint8_t src = b.registerTable[n->identifier];
                uint8_t tmp = allocGP(b);
                emitInstr(b, OP_MOV, tmp, src, 0);
                return tmp;
            }
        }
        return 0;
    }
    void compileStmt(BCData& b, parser::ASTNode* node) {
        switch (node->type) {
            case parser::NODE_VAR_DECL: {
                auto* n = static_cast<parser::VarDeclNode*>(node);
                uint8_t reg = compileExpr(b, n->expression);
                b.registerTable.insert({n->identifier, reg});
                break;
            }
            case parser::NODE_VAR_ASSIGN: {

            }
        }
    }

    void compileProgram(BCData& b, parser::ProgramNode* node) {
        for (parser::ASTNode* stmt : node->program) {
            compileStmt(b, stmt);
        }
    }
}
