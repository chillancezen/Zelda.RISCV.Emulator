/*
 * Copyright (c) 2019-2020 Jie Zheng
 */

#include <translation.h>
#include <util.h>
#include <debug.h>
#include <hart_util.h>

__attribute__((unused)) static void
ebreak_callback(struct hart * hartptr)
{
#if defined(NATIVE_DEBUGER)
    enter_vmm_dbg_shell(hartptr, 0);
#endif
}

static void
riscv_ebreak_translator(struct decoding * dec, struct prefetch_blob * blob,
                        uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(ebreak_instruction, blob);
    BEGIN_TRANSLATION(ebreak_instruction);
        __asm__ volatile("movq %%r12, %%rdi;"
                         "movq $ebreak_callback, %%rax;"
                         SAVE_GUEST_CONTEXT_SWITCH_REGS()
                         "call *%%rax;"
                         RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                         PROCEED_TO_NEXT_INSTRUCTION()
                         END_INSTRUCTION(ebreak_instruction)
                         :
                         :
                         :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*dummy*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(ebreak_instruction);
        BEGIN_PARAM(ebreak_instruction)
            instruction_linear_address
        END_PARAM()
    COMMIT_TRANSLATION(ebreak_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

__attribute__((unused)) static void
mret_callback(struct hart * hartptr)
{
    assert_hart_running_in_mmode(hartptr);
    adjust_mstatus_upon_mret(hartptr);
    adjust_pc_upon_mret(hartptr);
    // translation cache must be flushed, because adjusted privilege level may
    // diff in addressing space
    flush_translation_cache(hartptr);
    
}

static void
riscv_mret_translator(struct decoding * dec, struct prefetch_blob * blob,
                      uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(mret_instruction, blob);
    BEGIN_TRANSLATION(mret_instruction);
    __asm__ volatile("movq %%r12, %%rdi;"
                     "movq $mret_callback, %%rax;"
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;"
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                     TRAP_TO_VMM(mret_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32()
        END_PARAM_SCHEMA()
    END_TRANSLATION(mret_instruction);
        BEGIN_PARAM(mret_instruction)
            instruction_linear_address
        END_PARAM()
    COMMIT_TRANSLATION(mret_instruction, hartptr, instruction_linear_address);
    blob->is_to_stop = 1;
}

__attribute__((unused)) static void
sfence_vma_callback(struct hart * hartptr)
{
    // FLUSH TLB

    // and finlally, flush translation cache
    flush_translation_cache(hartptr);
}

static void
riscv_sfence_vma_translator(struct decoding * dec, struct prefetch_blob * blob,
                            uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(sfence_vma_instruction, blob);
    BEGIN_TRANSLATION(sfence_vma_instruction);
    __asm__ volatile("movq %%r12, %%rdi;"
                     "movq $sfence_vma_callback, %%rax;"
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;"
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     TRAP_TO_VMM(sfence_vma_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32()
        END_PARAM_SCHEMA()
    END_TRANSLATION(sfence_vma_instruction);
        BEGIN_PARAM(sfence_vma_instruction)
            instruction_linear_address
        END_PARAM()
    COMMIT_TRANSLATION(sfence_vma_instruction, hartptr, instruction_linear_address);
    blob->is_to_stop = 1;
}

static void
riscv_funct3_000_translator(struct decoding * dec, struct prefetch_blob * blob,
                            uint32_t instruction)
{
    if (((dec->imm >> 5) & 0x7f) == 0x9) {
        riscv_sfence_vma_translator(dec, blob, instruction);
    } else if (dec->rs2_index == 0x1) {
        riscv_ebreak_translator(dec, blob, instruction); 
    } else if (dec->rs2_index == 0x2) {
        if ((dec->imm >> 5) == 0x18) {
            riscv_mret_translator(dec, blob, instruction);
        } else {
            __not_reach();
        }
    }else {
        log_fatal("can not translate:0x%x at 0x%x\n", instruction, blob->next_instruction_to_fetch);
        __not_reach();
    }
}


static instruction_sub_translator per_funct3_handlers[8];


void
riscv_supervisor_level_instructions_translation_entry(struct prefetch_blob * blob,
                                                      uint32_t instruction)
{
    struct decoding dec;
    // FIXED: only funct3:000 is encoded with type-S, others are not.
    if (!((instruction >> 12) & 0x7)) {
        instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_S);
    } else {
        instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_I);
    }
    ASSERT(per_funct3_handlers[dec.funct3]);
    per_funct3_handlers[dec.funct3](&dec, blob, instruction);
}


void
riscv_generic_csr_instructions_translator(struct decoding * dec,
                                          struct prefetch_blob * blob,
                                          uint32_t instruction);

__attribute__((constructor)) static void
supervisor_level_constructor(void)
{
    memset(per_funct3_handlers, 0x0, sizeof(per_funct3_handlers));
    per_funct3_handlers[0x0] = riscv_funct3_000_translator;
    per_funct3_handlers[0x1] = riscv_generic_csr_instructions_translator;
    per_funct3_handlers[0x2] = riscv_generic_csr_instructions_translator;
    per_funct3_handlers[0x3] = riscv_generic_csr_instructions_translator;
    per_funct3_handlers[0x5] = riscv_generic_csr_instructions_translator;
    per_funct3_handlers[0x6] = riscv_generic_csr_instructions_translator;
    per_funct3_handlers[0x7] = riscv_generic_csr_instructions_translator;
}

