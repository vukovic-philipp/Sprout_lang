#pragma once
#include <cstdint>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdio>
#include <opcodes.h>
#include "parser.h"

namespace sprout::codegen {
    struct BCData {
        std::unordered_map<std::string, uint8_t> registerTable;
        uint8_t constRegNext = 24;
        uint8_t gpRegNext = 64;
        std::vector<uint8_t> byteCode;
    };

    inline void emitInstr(BCData& b, uint8_t OPCODE, uint8_t A, uint8_t B, uint8_t C) {
        b.byteCode.push_back(OPCODE);
        b.byteCode.push_back(A);
        b.byteCode.push_back(B);
        b.byteCode.push_back(C);
    }
    inline void emitImm(BCData& b, uint32_t imm) {
        for (int i = 3; i >= 0; --i) {
            b.byteCode.push_back(static_cast<uint8_t>((imm >> (i * 8)) & 0xFF));
        }
    }

    inline void emitImm64(BCData& b, uint64_t imm) {
        for (int i = 7; i >= 0; --i) {
            b.byteCode.push_back(static_cast<uint8_t>((imm >> (i * 8)) & 0xFF));
        }
    }

    inline uint8_t allocGP(BCData& b) {
        if (b.gpRegNext > 255) throw std::runtime_error("GP register allocation overflow!");
        return b.gpRegNext++;
    }

    inline void deallocGP(BCData& b, uint8_t reg) {
        if (b.gpRegNext - 1 == reg) b.gpRegNext -= 1;
    }

    inline uint8_t allocConst(BCData &b) {
        if (b.constRegNext > 63) throw std::runtime_error("Const register allocation overflow!");
        return b.constRegNext++;
    }

    inline void deallocConst(BCData& b, uint8_t reg) {
        if (b.constRegNext - 1 == reg) b.constRegNext -= 1;
    }

    inline void beginFunction(BCData& b) {
        b.gpRegNext = 64;
        b.constRegNext = 64;
    }

    inline void emitLoad64(BCData& b, uint8_t reg, uint64_t imm) {
        emitInstr(b, OP_LOAD64, reg, 0, 0);
        emitImm64(b, imm);
    }

        void compileProgram(BCData& b, parser::ProgramNode* node);

    inline const char* opcodeName(uint8_t op) {
        switch (op) {
            case OP_ADD:              return "ADD";
            case OP_SUB:              return "SUB";
            case OP_MUL:              return "MUL";
            case OP_DIV:              return "DIV";
            case OP_END:              return "END";
            case OP_LOAD64:           return "LOAD64";
            case OP_CMP:              return "CMP";
            case OP_DEBUG_RETURN:     return "DEBUG_RETURN";
            case OP_JE:               return "JE";
            case OP_JNE:              return "JNE";
            case OP_JL:               return "JL";
            case OP_JG:               return "JG";
            case OP_CALL:             return "CALL";
            case OP_RET:              return "RET";
            case OP_PUSH:             return "PUSH";
            case OP_POP:              return "POP";
            case OP_INSERT_INTO_STACK:return "INSERT_INTO_STACK";
            case OP_READ_FROM_STACK:  return "READ_FROM_STACK";
            case OP_MOV:              return "MOV";
            case OP_ARR_INIT:         return "ARR_INIT";
            case OP_ARR_INSERT:       return "ARR_INSERT";
            case OP_ARR_READ:         return "ARR_READ";
            default:                  return "UNKNOWN";
        }
    }

    inline void disassemble(const BCData& b) {
        const auto& code = b.byteCode;
        size_t i = 0;
        while (i < code.size()) {
            size_t addr = i;
            uint8_t op = code[i++];

            if (i + 2 >= code.size()) {
                std::printf("%04zx: %s (truncated)\n", addr, opcodeName(op));
                break;
            }

            uint8_t A = code[i++];
            uint8_t B = code[i++];
            uint8_t C = code[i++];

            if (op == OP_LOAD64) {
                // Next 8 bytes are the 64-bit immediate
                if (i + 8 > code.size()) {
                    std::printf("%04zx: %-20s r%u  (immediate truncated)\n", addr, "LOAD64", A);
                    break;
                }
                uint64_t imm = 0;
                for (int j = 7; j >= 0; --j) {
                    imm |= static_cast<uint64_t>(code[i++]) << (j * 8);
                }
                std::printf("%04zx: %-20s r%-3u  0x%016llX\n", addr, "LOAD64", A,
                            static_cast<unsigned long long>(imm));
            } else {
                std::printf("%04zx: %-20s r%-3u  r%-3u  r%-3u\n", addr, opcodeName(op), A, B, C);
            }
        }
    }
}
