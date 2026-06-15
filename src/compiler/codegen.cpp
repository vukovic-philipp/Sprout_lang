#include "codegen.hpp"

#include "parser.h"
#include "values.h"

namespace codegen {
uint8_t compileExpr(BCData& b, sprout::parser::ASTNode* node) {
    switch (node->type) {
        case sprout::parser::NODE_INT_LITERAL: {
            auto* n = static_cast<sprout::parser::IntLiteralNode*>(node);
            uint8_t reg = allocGP(b);
            emitLoad64(b, reg, encodeInt(n->number));
            return reg;
        }
    }
}
}
