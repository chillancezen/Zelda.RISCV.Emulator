/*
 * Copyright (c) 2019 Jie Zheng
 */
#ifndef _TRANSLATION_H
#define _TRANSLATION_H
#include <vm.h>

enum RISCV_OPCODE {
    RISCV_OPCODE_LUI = 0x37,
    RISCV_OPCODE_AUIPC = 0x17,
    RISCV_OPCODE_JAL = 0x6f,
    RISCV_OPCODE_JARL = 0x67,
    RISCV_OPCODE_BRANCH = 0x63,
    RISCV_OPCODE_LOAD = 0x03,
    RISCV_OPCODE_STORE = 0x23,
    RISCV_OPCODE_OP_IMM = 0x13,
    RISCV_OPCODE_OP = 0x33,
    RISCV_OPCODE_FENCE = 0x0f
};

enum INSTRUCTION_ENCODING_TYPE {
    ENCODING_TYPE_UNKNOWN = 0,
    ENCODING_TYPE_U = 1,
    ENCODING_TYPE_I,
};

struct decoding {
    enum INSTRUCTION_ENCODING_TYPE instr_encoding_type;
    uint8_t opcode;
    uint8_t rd_index;
    uint32_t imm;
};

void
vmresume(struct hart * hartptr);

// before entering translation cache, the RBX is set to the address of the hart
// registers group
#define BEGIN_TRANSLATION(indicator)                                           \
__asm__ volatile ("movq $" #indicator "_translation_end, %%rdx;"               \
                  "jmpq *%%rdx;"                                               \
                  :                                                            \
                  :                                                            \
                  :"%rdx");                                                    \
__asm__ volatile (#indicator  "_translation_begin:\n")



#define END_TRANSLATION(indicator)                                             \
__asm__ volatile (#indicator  "_translation_end:\n")

#define COMMIT_TRANSLATION(indicator)                                          \
{                                                                              \
    uint64_t translation_begin_addr = 0;                                       \
    uint64_t translation_end_addr = 0;                                         \
    __asm__ volatile("movq $" #indicator "_translation_begin, %%rax;"          \
                     "movq $" #indicator "_translation_end, %%rdx;"            \
                     :"=a"(translation_begin_addr),                            \
                      "=d"(translation_end_addr)                               \
                     :                                                         \
                     :"memory");                                               \
}

#define PIC_PARAM(index) "(11f + 4 * "#index")(%%rip)"
#define END_INSTRUCTION(indicator)                                             \
                     "jmp "#indicator"_translation_end;"


#define BEGIN_PARAM_SCHEMA()                                                   \
__asm__ volatile(".align 4;"                                                   \
                 "11:"

#define PARAM()                                                                \
                 ".int 0x90909090;"

#define END_PARAM_SCHEMA()                                                     \
                 );


#define BEGIN_PARAM(indicator)                                                 \
__attribute__((unused)) int indicator##_params[] = {                           \


#define END_PARAM()                                                            \
};



#endif
