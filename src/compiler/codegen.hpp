#pragma once
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <opcodes.h>

namespace codegen {
    struct BCData {
        std::unordered_map<std::string, uint8_t> registerTable;
        uint8_t constRegNext = 24;
        uint8_t gpRegNext = 64;
        std::vector<uint8_t> byteCode;
    };

    void emitInstr(BCData& b, uint8_t OPCODE, uint8_t A, uint8_t B, uint8_t C) {
        b.byteCode.push_back(OPCODE);
        b.byteCode.push_back(A);
        b.byteCode.push_back(B);
        b.byteCode.push_back(C);
    }
    void emitImm(BCData& b, uint32_t imm) {
        for (int i = 3; i >= 0; --i) {
            b.byteCode.push_back(static_cast<uint8_t>((imm >> (i * 8)) & 0xFF));
        }
    }

    void emitImm64(BCData& b, uint64_t imm) {
        for (int i = 7; i >= 0; --i) {
            b.byteCode.push_back(static_cast<uint8_t>((imm >> (i * 8)) & 0xFF));
        }
    }

    uint8_t allocGP(BCData& b) {
        if (b.gpRegNext > 255) throw std::runtime_error("GP register allocation overflow!");
        return b.gpRegNext++;
    }

    void deallocGP(BCData& b, uint8_t reg) {
        if (b.gpRegNext - 1 == reg) b.gpRegNext -= 1;
    }

    uint8_t allocConst(BCData &b) {
        if (b.constRegNext > 63) throw std::runtime_error("GP register allocation overflow!");
        return b.constRegNext++;
    }

    void deallocConst(BCData& b, uint8_t reg) {
        if (b.constRegNext - 1 == reg) b.constRegNext -= 1;
    }

    void beginFunction(BCData& b) {
        b.gpRegNext = 64;
        b.constRegNext = 64;
    }

    void emitLoad64(BCData& b, uint8_t reg, uint64_t imm) {
        emitInstr(b, OP_LOAD64, reg, 0, 0);
        emitImm64(b, imm);
    }

}
