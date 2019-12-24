/*
 * Copyright (c) 2019 Jie Zheng
 *  
 *      This file extends supervisor-level instructions translation
 */

#include <translation.h>
#include <util.h>


void
riscv_generic_csr_callback(struct hart * hartptr, uint64_t instruction)
{
    ASSERT(hartptr->hart_magic == HART_MAGIC_WORD);
    ASSERT((instruction & 0x7f) ==  RISCV_OPCODE_SUPERVISOR_LEVEL);
    struct decoding dec;
    instruction_decoding_per_type(&dec, (uint32_t)instruction, ENCODING_TYPE_I);
}

void
riscv_generic_csr_instructions_translator(struct decoding * dec,
                                          struct prefetch_blob * blob,
                                          uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(csr_instructions, blob);
    BEGIN_TRANSLATION(csr_instructions);
    __asm__ volatile("movq %%r12, %%rdi;"
                     "movl "PIC_PARAM(0)", %%esi;"
                     "movq $riscv_generic_csr_callback, %%rax;"
                     "call *%%rax;"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(csr_instructions)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() //the instruction itself
        END_PARAM_SCHEMA()
    END_TRANSLATION(csr_instructions);
        BEGIN_PARAM(csr_instructions)
            instruction
        END_PARAM()
    COMMIT_TRANSLATION(csr_instructions, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

