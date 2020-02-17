/*
 * Copyright (c) 2020 Jie Zheng
 */

#include <translation.h>
#include <util.h>
#include <mmu.h>

void
amoadd_slowpath(struct hart * hartptr, uint8_t rs1_index, uint8_t rs2_index,
                uint8_t rd_index)
{
    uint32_t * regs = (uint32_t *)&hartptr->registers;
    // MUST make a copy of rs1 and rs2. because if rd equals any of them, things
    // getting wrong.
    uint32_t rs1 = regs[rs1_index];
    uint32_t rs2 = regs[rs2_index];
    regs[rd_index]= mmu_read32_aligned(hartptr, rs1);
    mmu_write32_aligned(hartptr, rs1, regs[rd_index] + rs2);
}

static void
riscv_amoadd_translator(struct decoding * dec, struct prefetch_blob * blob,
                        uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(amoadd_instruction, blob);
    BEGIN_TRANSLATION(amoadd_instruction);
    __asm__ volatile(
                    // RDI: hartptr
                    // RSI: rs1_index,
                    // RDX: rs2_index,
                    // RCX: rd_index
                     "movq %%r12, %%rdi;"
                     "movl "PIC_PARAM(0)", %%esi;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "movl "PIC_PARAM(2)", %%ecx;"
                     "movq $amoadd_slowpath, %%rax;"
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;"
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(amoadd_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(amoadd_instruction);
        BEGIN_PARAM(amoadd_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(amoadd_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static instruction_sub_translator per_funct5_handlers[32];

void
riscv_amo_instructions_translation_entry(struct prefetch_blob * blob,
                                         uint32_t instruction)
{
    struct decoding dec;
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_R);
    uint8_t funct5 = dec.funct7 >> 2;
    if (!per_funct5_handlers[funct5]) {
        printf("instruction_linear_address:%x\n", blob->next_instruction_to_fetch);
    }
    ASSERT(per_funct5_handlers[funct5]);
    per_funct5_handlers[funct5](&dec, blob, instruction);
}


__attribute__((constructor)) static void
amo_constructor(void)
{
    memset(per_funct5_handlers, 0x0, sizeof(per_funct5_handlers));
    per_funct5_handlers[0] = riscv_amoadd_translator;
}
