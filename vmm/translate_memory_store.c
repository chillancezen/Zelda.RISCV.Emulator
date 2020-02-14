/*
 * Copyright (c) 2019 Jie Zheng
 */

#include <translation.h>
#include <util.h>
#include <string.h>
#include <mmu.h>


static void
riscv_sb_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_offset = sign_extend32(dec->imm, 11);
    PRECHECK_TRANSLATION_CACHE(sb_instruction, blob);
    BEGIN_TRANSLATION(sb_instruction);
    __asm__ volatile("movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "addl %%edx, %%esi;"          // ESI: memory store target location
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%edx;"        // EDX: mmeory store source value
                     "andl $0xff, %%edx;"
                     "movq %%r12, %%rdi;"
                     "movq $mmu_write8, %%rax;"
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;"
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(sb_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed offset*/
            PARAM32() /*rs1_index: memory base register*/
            PARAM32() /*rs2_index: source operand register*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(sb_instruction);
        BEGIN_PARAM(sb_instruction)
            signed_offset,
            dec->rs1_index,
            dec->rs2_index
        END_PARAM()
    COMMIT_TRANSLATION(sb_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}


static void
riscv_sh_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_offset = sign_extend32(dec->imm, 11);
    PRECHECK_TRANSLATION_CACHE(sh_instruction, blob);
    BEGIN_TRANSLATION(sh_instruction);
    __asm__ volatile("movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "addl %%edx, %%esi;"          // ESI: memory store target location
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%edx;"        // EDX: mmeory store source value
                     "andl $0xffff, %%edx;"
                     "movq %%r12, %%rdi;"
                     "movq $mmu_write16, %%rax;"
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;"
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(sh_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed offset*/
            PARAM32() /*rs1_index: memory base register*/
            PARAM32() /*rs2_index: source operand register*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(sh_instruction);
        BEGIN_PARAM(sh_instruction)
            signed_offset,
            dec->rs1_index,
            dec->rs2_index
        END_PARAM()
    COMMIT_TRANSLATION(sh_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}


static void
riscv_sw_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_offset = sign_extend32(dec->imm, 11);
    PRECHECK_TRANSLATION_CACHE(sw_instruction, blob);
    BEGIN_TRANSLATION(sw_instruction);
    __asm__ volatile("movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "addl %%edx, %%esi;"          // ESI: memory store target location
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%edx;"        // EDX: mmeory store source value
                     "movq %%r12, %%rdi;"
                     "movq $mmu_write32, %%rax;"
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;"
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(sw_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed offset*/
            PARAM32() /*rs1_index: memory base register*/
            PARAM32() /*rs2_index: source operand register*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(sw_instruction);
        BEGIN_PARAM(sw_instruction)
            signed_offset,
            dec->rs1_index,
            dec->rs2_index
        END_PARAM()
    COMMIT_TRANSLATION(sw_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static instruction_sub_translator per_funct3_handlers[8];

void
riscv_memory_store_instructions_translation_entry(struct prefetch_blob * blob,
                                                  uint32_t instruction)
{
    struct decoding dec;
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_S);
    ASSERT(per_funct3_handlers[dec.funct3]);
    per_funct3_handlers[dec.funct3](&dec, blob, instruction);
}


__attribute__((constructor)) static void
memory_store_constructor(void)
{
    memset(per_funct3_handlers, 0x0, sizeof(per_funct3_handlers));
    per_funct3_handlers[0] = riscv_sb_translator;
    per_funct3_handlers[1] = riscv_sh_translator;
    per_funct3_handlers[2] = riscv_sw_translator;
}
