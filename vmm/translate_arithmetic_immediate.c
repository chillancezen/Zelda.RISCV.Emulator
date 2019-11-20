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
    per_funct3_handlers[0x2] = riscv_stli_translator;
    per_funct3_handlers[0x3] = riscv_stliu_translator;
}
