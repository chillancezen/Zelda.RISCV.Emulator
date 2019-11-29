/*
 * Copyright (c) 2019 Jie Zheng
 */

#include <translation.h>
#include <util.h>
#include <string.h>

static void
riscv_beq_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t branch_taken_target = instruction_linear_address +
                                  sign_extend32(dec->imm << 1, 12);
    PRECHECK_TRANSLATION_CACHE(beq_instruction, blob);
    BEGIN_TRANSLATION(beq_instruction);
        __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%esi;"
                         "movl "PIC_PARAM(1)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%edi;"
                         "movl (%%r14), %%edx;" // <=== Let edx always keep the branch target
                         "addl $0x4, %%edx;"
                         "cmpl %%esi, %%edi;"
                         "jne 1f;"
                         "movl "PIC_PARAM(2)", %%edx;"
                         "1:"
                         "movl %%edx, (%%r14);"
                         TRAP_TO_VMM()
                         :
                         :
                         :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*branch taken target*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(beq_instruction);
        BEGIN_PARAM(beq_instruction)
            dec->rs1_index,
            dec->rs2_index,
            branch_taken_target 
        END_PARAM()
    COMMIT_TRANSLATION(beq_instruction, hartptr, instruction_linear_address);
    blob->is_to_stop = 1;
}

static void
riscv_bne_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t branch_taken_target = instruction_linear_address +
                                  sign_extend32(dec->imm << 1, 12);
    PRECHECK_TRANSLATION_CACHE(bne_instruction, blob);
    BEGIN_TRANSLATION(bne_instruction);
        __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%esi;"
                         "movl "PIC_PARAM(1)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%edi;"
                         "movl (%%r14), %%edx;" // <=== Let edx always keep the branch target
                         "addl $0x4, %%edx;"
                         "cmpl %%esi, %%edi;"
                         "je 1f;"
                         "movl "PIC_PARAM(2)", %%edx;"
                         "1:"
                         "movl %%edx, (%%r14);"
                         TRAP_TO_VMM()
                         :
                         :
                         :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*branch taken target*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(bne_instruction);
        BEGIN_PARAM(bne_instruction)
            dec->rs1_index,
            dec->rs2_index,
            branch_taken_target
        END_PARAM()
    COMMIT_TRANSLATION(bne_instruction, hartptr, instruction_linear_address);
    blob->is_to_stop = 1;
}


static void
riscv_blt_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t branch_taken_target = instruction_linear_address +
                                  sign_extend32(dec->imm << 1, 12);
    PRECHECK_TRANSLATION_CACHE(blt_instruction, blob);
    BEGIN_TRANSLATION(blt_instruction);
        __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%esi;"
                         "movl "PIC_PARAM(1)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%edi;"
                         "movl (%%r14), %%edx;" // <=== Let edx always keep the branch target
                         "addl $0x4, %%edx;"
                         "cmpl %%edi, %%esi;" // rs1 - rs2 : rs1 < rs2
                         "jge 1f;"
                         "movl "PIC_PARAM(2)", %%edx;"
                         "1:"
                         "movl %%edx, (%%r14);"
                         TRAP_TO_VMM()
                         :
                         :
                         :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*branch taken target*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(blt_instruction);
        BEGIN_PARAM(blt_instruction)
            dec->rs1_index,
            dec->rs2_index,
            branch_taken_target
        END_PARAM()
    COMMIT_TRANSLATION(blt_instruction, hartptr, instruction_linear_address);
    blob->is_to_stop = 1;
}

static void
riscv_bltu_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t branch_taken_target = instruction_linear_address +
                                  sign_extend32(dec->imm << 1, 12);
    PRECHECK_TRANSLATION_CACHE(bltu_instruction, blob);
    BEGIN_TRANSLATION(bltu_instruction);
        __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%esi;"
                         "movl "PIC_PARAM(1)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%edi;"
                         "movl (%%r14), %%edx;" // <=== Let edx always keep the branch target
                         "addl $0x4, %%edx;"
                         "cmpl %%edi, %%esi;" // rs1 - rs2 : rs1 < rs2
                         "jae 1f;"
                         "movl "PIC_PARAM(2)", %%edx;"
                         "1:"
                         "movl %%edx, (%%r14);"
                         TRAP_TO_VMM()
                         :
                         :
                         :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*branch taken target*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(bltu_instruction);
        BEGIN_PARAM(bltu_instruction)
            dec->rs1_index,
            dec->rs2_index,
            branch_taken_target
        END_PARAM()
    COMMIT_TRANSLATION(bltu_instruction, hartptr, instruction_linear_address);
    blob->is_to_stop = 1;
}

static void
riscv_bge_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t branch_taken_target = instruction_linear_address +
                                  sign_extend32(dec->imm << 1, 12);
    PRECHECK_TRANSLATION_CACHE(bge_instruction, blob);
    BEGIN_TRANSLATION(bge_instruction);
        __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%esi;"
                         "movl "PIC_PARAM(1)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%edi;"
                         "movl (%%r14), %%edx;" // <=== Let edx always keep the branch target
                         "addl $0x4, %%edx;"
                         "cmpl %%edi, %%esi;" // rs1 - rs2 : rs1 < rs2
                         "jl 1f;"
                         "movl "PIC_PARAM(2)", %%edx;"
                         "1:"
                         "movl %%edx, (%%r14);"
                         TRAP_TO_VMM()
                         :
                         :
                         :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*branch taken target*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(bge_instruction);
        BEGIN_PARAM(bge_instruction)
            dec->rs1_index,
            dec->rs2_index,
            branch_taken_target
        END_PARAM()
    COMMIT_TRANSLATION(bge_instruction, hartptr, instruction_linear_address);
    blob->is_to_stop = 1;
}

static void
riscv_bgeu_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t branch_taken_target = instruction_linear_address +
                                  sign_extend32(dec->imm << 1, 12);
    PRECHECK_TRANSLATION_CACHE(bgeu_instruction, blob);
    BEGIN_TRANSLATION(bgeu_instruction);
        __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%esi;"
                         "movl "PIC_PARAM(1)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%edi;"
                         "movl (%%r14), %%edx;" // <=== Let edx always keep the branch target
                         "addl $0x4, %%edx;"
                         "cmpl %%edi, %%esi;" // rs1 - rs2 : rs1 < rs2
                         "jb 1f;"
                         "movl "PIC_PARAM(2)", %%edx;"
                         "1:"
                         "movl %%edx, (%%r14);"
                         TRAP_TO_VMM()
                         :
                         :
                         :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*branch taken target*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(bgeu_instruction);
        BEGIN_PARAM(bgeu_instruction)
            dec->rs1_index,
            dec->rs2_index,
            branch_taken_target
        END_PARAM()
    COMMIT_TRANSLATION(bgeu_instruction, hartptr, instruction_linear_address);
    blob->is_to_stop = 1;
}

static instruction_sub_translator per_funct3_handlers[8];

void
riscv_branch_instructions_translation_entry(struct prefetch_blob * blob,
                                            uint32_t instruction)
{
    struct decoding dec;
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_B);
    assert(per_funct3_handlers[dec.funct3]);
    per_funct3_handlers[dec.funct3](&dec, blob, instruction);
}


__attribute__((constructor)) static void
branch_constructor(void)
{
    memset(per_funct3_handlers, 0x0, sizeof(per_funct3_handlers));
    per_funct3_handlers[0x0] = riscv_beq_translator;
    per_funct3_handlers[0x1] = riscv_bne_translator;
    per_funct3_handlers[0x4] = riscv_blt_translator;
    per_funct3_handlers[0x6] = riscv_bltu_translator;
    per_funct3_handlers[0x5] = riscv_bge_translator;
    per_funct3_handlers[0x7] = riscv_bgeu_translator;
}
