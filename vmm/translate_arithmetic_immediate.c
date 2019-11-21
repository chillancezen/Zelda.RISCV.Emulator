/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <translation.h>
#include <util.h>
#include <stdio.h>
#include <string.h>

typedef void
(*arithmetic_immediate_instruction_translator)(struct decoding * dec,
                                               struct prefetch_blob * blob,
                                               uint32_t);


static void
riscv_addi_translator(struct decoding * dec, struct prefetch_blob * blob,
                      uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_offset = sign_extend32(dec->imm, 11);
    PRECHECK_TRANSLATION_CACHE(addi_instruction, blob);
    BEGIN_TRANSLATION(addi_instruction);
        __asm__ volatile("movl "PIC_PARAM(1)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%eax;"
                         "movl "PIC_PARAM(0)", %%edx;"
                         "addl %%edx, %%eax;"
                         "movl "PIC_PARAM(2)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl %%eax, (%%rdx);"
                         PROCEED_TO_NEXT_INSTRUCTION()
                         END_INSTRUCTION(addi_instruction)
                         :
                         :
                         :"memory", "%rax", "%rdx");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed offset*/
            PARAM32() /*rs1 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(addi_instruction);
        BEGIN_PARAM(addi_instruction)
            signed_offset,
            dec->rs1_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(addi_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_stli_translator(struct decoding * dec, struct prefetch_blob * blob,
                      uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_imm = sign_extend32(dec->imm, 11);
    PRECHECK_TRANSLATION_CACHE(stli_instruction, blob);
    BEGIN_TRANSLATION(stli_instruction);
    __asm__ volatile("xor %%ecx, %%ecx;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "cmpl "PIC_PARAM(0)", %%eax;"
                     "setl %%cl;"
                     "movzbl %%cl, %%ecx;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%ecx, (%%rdx);"
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(stli_instruction)
                     :
                     :
                     :"memory", "%rax", "%rdx", "%rcx");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed*/
            PARAM32() /*rs1 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(stli_instruction);
        BEGIN_PARAM(stli_instruction)
            signed_imm,
            dec->rs1_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(stli_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_stliu_translator(struct decoding * dec, struct prefetch_blob * blob,
                      uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_imm = sign_extend32(dec->imm, 11);
    PRECHECK_TRANSLATION_CACHE(stliu_instruction, blob);
    BEGIN_TRANSLATION(stliu_instruction);
    __asm__ volatile("xor %%ecx, %%ecx;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "cmpl "PIC_PARAM(0)", %%eax;"
                     "setb %%cl;"
                     "movzbl %%cl, %%ecx;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%ecx, (%%rdx);"
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(stliu_instruction)
                     :
                     :
                     :"memory", "%rax", "%rdx", "%rcx");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed*/
            PARAM32() /*rs1 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(stliu_instruction);
        BEGIN_PARAM(stliu_instruction)
            signed_imm,
            dec->rs1_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(stliu_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_xori_translator(struct decoding * dec, struct prefetch_blob * blob,
                      uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_imm = sign_extend32(dec->imm, 11);
    PRECHECK_TRANSLATION_CACHE(xori_instruction, blob);
    BEGIN_TRANSLATION(xori_instruction);
    __asm__ volatile("xor %%ecx, %%ecx;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "xorl %%edx, %%eax;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(xori_instruction)
                     :
                     :
                     :"memory", "%rax", "%rdx");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed*/
            PARAM32() /*rs1 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(xori_instruction);
        BEGIN_PARAM(xori_instruction)
            signed_imm,
            dec->rs1_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(xori_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_ori_translator(struct decoding * dec, struct prefetch_blob * blob,
                      uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_imm = sign_extend32(dec->imm, 11);
    PRECHECK_TRANSLATION_CACHE(ori_instruction, blob);
    BEGIN_TRANSLATION(ori_instruction);
    __asm__ volatile("xor %%ecx, %%ecx;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "orl %%edx, %%eax;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(ori_instruction)
                     :
                     :
                     :"memory", "%rax", "%rdx");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed*/
            PARAM32() /*rs1 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(ori_instruction);
        BEGIN_PARAM(ori_instruction)
            signed_imm,
            dec->rs1_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(ori_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_andi_translator(struct decoding * dec, struct prefetch_blob * blob,
                      uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_imm = sign_extend32(dec->imm, 11);
    PRECHECK_TRANSLATION_CACHE(andi_instruction, blob);
    BEGIN_TRANSLATION(andi_instruction);
    __asm__ volatile("xor %%ecx, %%ecx;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "andl %%edx, %%eax;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(andi_instruction)
                     :
                     :
                     :"memory", "%rax", "%rdx");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed*/
            PARAM32() /*rs1 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(andi_instruction);
        BEGIN_PARAM(andi_instruction)
            signed_imm,
            dec->rs1_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(andi_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_slli_translator(struct decoding * dec, struct prefetch_blob * blob,
                      uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(slli_instruction, blob);
    BEGIN_TRANSLATION(slli_instruction);
    __asm__ volatile("xor %%ecx, %%ecx;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "movl "PIC_PARAM(0)", %%ecx;"
                     "andl $0x1f, %%ecx;"
                     "shl %%cl, %%eax;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(slli_instruction)
                     :
                     :
                     :"memory", "%rax", "%rdx", "%rcx");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed*/
            PARAM32() /*rs1 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(slli_instruction);
        BEGIN_PARAM(slli_instruction)
            dec->imm,
            dec->rs1_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(slli_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_right_shift_translator(struct decoding * dec, struct prefetch_blob * blob,
                             uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;

    if (!(dec->imm & 0x400)) {
        PRECHECK_TRANSLATION_CACHE(srli_instruction, blob);
        BEGIN_TRANSLATION(srli_instruction);
        __asm__ volatile("xor %%ecx, %%ecx;"
                         "movl "PIC_PARAM(1)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%eax;"
                         "movl "PIC_PARAM(0)", %%ecx;"
                         "andl $0x1f, %%ecx;"
                         "shr %%cl, %%eax;"
                         "movl "PIC_PARAM(2)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl %%eax, (%%rdx);"
                         PROCEED_TO_NEXT_INSTRUCTION()
                         END_INSTRUCTION(srli_instruction)
                         :
                         :
                         :"memory", "%rax", "%rdx", "%rcx");
            BEGIN_PARAM_SCHEMA()
                PARAM32() /*imm: signed*/
                PARAM32() /*rs1 index*/
                PARAM32() /*rd index*/
            END_PARAM_SCHEMA()
        END_TRANSLATION(srli_instruction);
            BEGIN_PARAM(srli_instruction)
                dec->imm,
                dec->rs1_index,
                dec->rd_index
            END_PARAM()
        COMMIT_TRANSLATION(srli_instruction, hartptr, instruction_linear_address);
    } else {
        PRECHECK_TRANSLATION_CACHE(srai_instruction, blob);
        BEGIN_TRANSLATION(srai_instruction);
        __asm__ volatile("xor %%ecx, %%ecx;"
                         "movl "PIC_PARAM(1)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl (%%rdx), %%eax;"
                         "movl "PIC_PARAM(0)", %%ecx;"
                         "andl $0x1f, %%ecx;"
                         "sar %%cl, %%eax;"
                         "movl "PIC_PARAM(2)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl %%eax, (%%rdx);"
                         PROCEED_TO_NEXT_INSTRUCTION()
                         END_INSTRUCTION(srai_instruction)
                         :
                         :
                         :"memory", "%rax", "%rdx", "%rcx");
            BEGIN_PARAM_SCHEMA()
                PARAM32() /*imm: signed*/
                PARAM32() /*rs1 index*/
                PARAM32() /*rd index*/
            END_PARAM_SCHEMA()
        END_TRANSLATION(srai_instruction);
            BEGIN_PARAM(srai_instruction)
                dec->imm,
                dec->rs1_index,
                dec->rd_index
            END_PARAM()
        COMMIT_TRANSLATION(srai_instruction, hartptr, instruction_linear_address);
    }
    blob->next_instruction_to_fetch += 4;
}

static arithmetic_immediate_instruction_translator per_funct3_handlers[8];

void
riscv_arithmetic_immediate_instructions_translation_entry(struct prefetch_blob * blob,
                                                          uint32_t instruction)
{


    struct decoding dec;
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_I);
    assert(per_funct3_handlers[dec.funct3]);
    per_funct3_handlers[dec.funct3](&dec, blob, instruction);
}

__attribute__((constructor)) static void
arithmetic_immediate_constructor(void)
{
    memset(per_funct3_handlers, 0x0, sizeof(per_funct3_handlers));
    per_funct3_handlers[0x0] = riscv_addi_translator;
    per_funct3_handlers[0x1] = riscv_slli_translator;
    per_funct3_handlers[0x2] = riscv_stli_translator;
    per_funct3_handlers[0x3] = riscv_stliu_translator;
    per_funct3_handlers[0x4] = riscv_xori_translator;
    per_funct3_handlers[0x5] = riscv_right_shift_translator;
    per_funct3_handlers[0x6] = riscv_ori_translator;
    per_funct3_handlers[0x7] = riscv_andi_translator;
}
