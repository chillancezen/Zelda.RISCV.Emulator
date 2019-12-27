/*
 * Copyright (c) 2019 Jie Zheng
 */

#include <translation.h>
#include <util.h>
#include <string.h>
#include <mmu.h>

__attribute__((unused)) static uint8_t
mmu_read8(struct hart * hartptr, uint32_t location)
{
    return vmread8(hartptr, location);
}

static void
riscv_lb_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)

{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_offset = sign_extend32(dec->imm, 11);

    PRECHECK_TRANSLATION_CACHE(lb_instruction, blob);
    BEGIN_TRANSLATION(lb_instruction);
    __asm__ volatile("movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "addl %%edx, %%esi;" // ESI: the memory location
                     "movq %%r12, %%rdi;"
                     "movq $mmu_read8, %%rax;"
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;" // EAX: the memory read from the location
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movsbl %%al, %%eax;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(lb_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed offset*/
            PARAM32() /*rs1_index*/
            PARAM32() /*rd_index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(lb_instruction);
        BEGIN_PARAM(lb_instruction)
            signed_offset,
            dec->rs1_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(lb_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_lbu_translator(struct decoding * dec, struct prefetch_blob * blob,
                     uint32_t instruction)

{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_offset = sign_extend32(dec->imm, 11);

    PRECHECK_TRANSLATION_CACHE(lbu_instruction, blob);
    BEGIN_TRANSLATION(lbu_instruction);
    __asm__ volatile("movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "addl %%edx, %%esi;" // ESI: the memory location
                     "movq %%r12, %%rdi;"
                     "movq $mmu_read8, %%rax;"
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;" // EAX: the memory read from the location
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movzbl %%al, %%eax;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(lbu_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed offset*/
            PARAM32() /*rs1_index*/
            PARAM32() /*rd_index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(lbu_instruction);
        BEGIN_PARAM(lbu_instruction)
            signed_offset,
            dec->rs1_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(lbu_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

__attribute__((unused)) static uint16_t
mmu_read16(struct hart * hartptr, uint32_t location)
{
    return vmread16(hartptr, location);
}

static void
riscv_lh_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)

{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_offset = sign_extend32(dec->imm, 11);

    PRECHECK_TRANSLATION_CACHE(lh_instruction, blob);
    BEGIN_TRANSLATION(lh_instruction);
    __asm__ volatile("movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "addl %%edx, %%esi;" // ESI: the memory location
                     "movq %%r12, %%rdi;"
                     "movq $mmu_read16, %%rax;"
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;" // EAX: the memory read from the location
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movswl %%ax, %%eax;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(lh_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed offset*/
            PARAM32() /*rs1_index*/
            PARAM32() /*rd_index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(lh_instruction);
        BEGIN_PARAM(lh_instruction)
            signed_offset,
            dec->rs1_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(lh_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_lhu_translator(struct decoding * dec, struct prefetch_blob * blob,
                     uint32_t instruction)

{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_offset = sign_extend32(dec->imm, 11);

    PRECHECK_TRANSLATION_CACHE(lhu_instruction, blob);
    BEGIN_TRANSLATION(lhu_instruction);
    __asm__ volatile("movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "addl %%edx, %%esi;" // ESI: the memory location
                     "movq %%r12, %%rdi;"
                     "movq $mmu_read16, %%rax;"
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;" // EAX: the memory read from the location
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movzwl %%ax, %%eax;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(lhu_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed offset*/
            PARAM32() /*rs1_index*/
            PARAM32() /*rd_index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(lhu_instruction);
        BEGIN_PARAM(lhu_instruction)
            signed_offset,
            dec->rs1_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(lhu_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

__attribute__((unused)) static uint32_t
mmu_read32(struct hart * hartptr, uint32_t location)
{
    return vmread32(hartptr, location);
}

static void
riscv_lw_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)

{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    int32_t signed_offset = sign_extend32(dec->imm, 11);

    PRECHECK_TRANSLATION_CACHE(lw_instruction, blob);
    BEGIN_TRANSLATION(lw_instruction);
    __asm__ volatile("movl "PIC_PARAM(1)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%esi;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "addl %%edx, %%esi;" // ESI: the memory location
                     "movq %%r12, %%rdi;"
                     "movq $mmu_read32, %%rax;"
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;" // EAX: the memory read from the location
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                     "movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(lw_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm: signed offset*/
            PARAM32() /*rs1_index*/
            PARAM32() /*rd_index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(lw_instruction);
        BEGIN_PARAM(lw_instruction)
            signed_offset,
            dec->rs1_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(lw_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static instruction_sub_translator per_funct3_handlers[8];

void
riscv_memory_load_instructions_translation_entry(struct prefetch_blob * blob,
                                                  uint32_t instruction)
{
    struct decoding dec;
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_I);
    ASSERT(per_funct3_handlers[dec.funct3]);
    per_funct3_handlers[dec.funct3](&dec, blob, instruction);
}


__attribute__((constructor)) static void
memory_load_constructor(void)
{
    memset(per_funct3_handlers, 0x0, sizeof(per_funct3_handlers));
    per_funct3_handlers[2] = riscv_lw_translator;
    per_funct3_handlers[1] = riscv_lh_translator;
    per_funct3_handlers[5] = riscv_lhu_translator;
    per_funct3_handlers[0] = riscv_lb_translator;
    per_funct3_handlers[4] = riscv_lbu_translator;
}
