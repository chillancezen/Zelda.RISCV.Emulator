/*
 * Copyright (c) 2019 Jie Zheng
 */

#include <translation.h>
#include <string.h>
#include <util.h>
static void
riscv_add_translator(struct decoding * dec, struct prefetch_blob * blob,
                     uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(add_instruction, blob);
    BEGIN_TRANSLATION(add_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "addl %%esi, %%eax;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(add_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(add_instruction);
        BEGIN_PARAM(add_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(add_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_sub_translator(struct decoding * dec, struct prefetch_blob * blob,
                     uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(sub_instruction, blob);
    BEGIN_TRANSLATION(sub_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "subl %%esi, %%eax;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(sub_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(sub_instruction);
        BEGIN_PARAM(sub_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(sub_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_add_sub_translator(struct decoding * dec, struct prefetch_blob * blob,
                         uint32_t instruction)
{
    if (dec->funct7 == 0x20) {
        riscv_sub_translator(dec, blob, instruction);
    } else {
        ASSERT(!dec->funct7);
        riscv_add_translator(dec, blob, instruction);
    }
}


static void
riscv_and_translator(struct decoding * dec, struct prefetch_blob * blob,
                     uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(and_instruction, blob);
    BEGIN_TRANSLATION(and_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "andl %%esi, %%eax;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(and_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(and_instruction);
        BEGIN_PARAM(and_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(and_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_or_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(or_instruction, blob);
    BEGIN_TRANSLATION(or_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "orl %%esi, %%eax;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(or_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(or_instruction);
        BEGIN_PARAM(or_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(or_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_xor_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(xor_instruction, blob);
    BEGIN_TRANSLATION(xor_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "xorl %%esi, %%eax;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(xor_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(xor_instruction);
        BEGIN_PARAM(xor_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(xor_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_slt_translator(struct decoding * dec, struct prefetch_blob * blob,
                     uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(slt_instruction, blob);
    BEGIN_TRANSLATION(slt_instruction);
    __asm__ volatile("xor %%ecx, %%ecx;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%edi;"
                     "cmpl %%edi, %%esi;"
                     "setl %%cl;"
                     "movzbl %%cl, %%ecx;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%ecx, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(slt_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(slt_instruction);
        BEGIN_PARAM(slt_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(slt_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_sltu_translator(struct decoding * dec, struct prefetch_blob * blob,
                      uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(sltu_instruction, blob);
    BEGIN_TRANSLATION(sltu_instruction);
    __asm__ volatile("xor %%ecx, %%ecx;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%edi;"
                     "cmpl %%edi, %%esi;"
                     "setb %%cl;"
                     "movzbl %%cl, %%ecx;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%ecx, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(sltu_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(sltu_instruction);
        BEGIN_PARAM(sltu_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(sltu_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_sll_translator(struct decoding * dec, struct prefetch_blob * blob,
                     uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(sll_instruction, blob);
    BEGIN_TRANSLATION(sll_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%ecx;"
                     "andl $0x1f, %%ecx;"
                     "shl %%cl, %%eax;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(sll_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(sll_instruction);
        BEGIN_PARAM(sll_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(sll_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}




static void
riscv_srl_translator(struct decoding * dec, struct prefetch_blob * blob,
                     uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(srl_instruction, blob);
    BEGIN_TRANSLATION(srl_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%ecx;"
                     "andl $0x1f, %%ecx;"
                     "shr %%cl, %%eax;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(srl_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(srl_instruction);
        BEGIN_PARAM(srl_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(srl_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_sra_translator(struct decoding * dec, struct prefetch_blob * blob,
                     uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(sra_instruction, blob);
    BEGIN_TRANSLATION(sra_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%eax;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%ecx;"
                     "andl $0x1f, %%ecx;"
                     "sar %%cl, %%eax;"
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $0x2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(sra_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(sra_instruction);
        BEGIN_PARAM(sra_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(sra_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_srl_sra_translator(struct decoding * dec, struct prefetch_blob * blob,
                         uint32_t instruction)
{
    if (dec->funct7 == 0x20) {
        riscv_sra_translator(dec, blob, instruction);
    } else {
        ASSERT(!dec->funct7);
        riscv_srl_translator(dec, blob, instruction);
    }
}
static instruction_sub_translator per_funct3_handlers[8];

void
riscv_arithmetic_instructions_class0_translation_entry(struct decoding * dec,
                                                       struct prefetch_blob * blob,
                                                       uint32_t instruction)
{
    ASSERT(per_funct3_handlers[dec->funct3]);
    per_funct3_handlers[dec->funct3](dec, blob, instruction);
    #if 0
    struct decoding dec;
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_R);
    ASSERT(per_funct3_handlers[dec.funct3]);
    per_funct3_handlers[dec.funct3](&dec, blob, instruction);
    #endif
}


__attribute__((constructor)) static void
arithmetic_constructor(void)
{
    memset(per_funct3_handlers, 0x0, sizeof(per_funct3_handlers));
    per_funct3_handlers[0x0] = riscv_add_sub_translator;
    per_funct3_handlers[0x7] = riscv_and_translator;
    per_funct3_handlers[0x6] = riscv_or_translator;
    per_funct3_handlers[0x4] = riscv_xor_translator;
    per_funct3_handlers[0x2] = riscv_slt_translator;
    per_funct3_handlers[0x3] = riscv_sltu_translator;
    per_funct3_handlers[0x1] = riscv_sll_translator;
    per_funct3_handlers[0x5] = riscv_srl_sra_translator;
}


