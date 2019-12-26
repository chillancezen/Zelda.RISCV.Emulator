/*
 * Copyright (c) 2019 Jie Zheng
 */

#include <translation.h>
#include <util.h>
#include <debug.h>

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
                         "call *%%rax;"
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

static void
riscv_funct3_000_translator(struct decoding * dec, struct prefetch_blob * blob,
                            uint32_t instruction)
{
    if (dec->rs2_index == 0x1) {
        riscv_ebreak_translator(dec, blob, instruction); 
    } else {
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

