/*
 * Copyright (c) 2019 Jie Zheng
 */

#include <translation.h>
#include <util.h>

static void
riscv_fence_i_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(fence_i_instruction, blob);
    BEGIN_TRANSLATION(fence_i_instruction);
        __asm__ volatile("movq %%r12, %%rdi;"
                         "movq $flush_translation_cache, %%rax;"
                         "call *%%rax;"
                         PROCEED_TO_NEXT_INSTRUCTION()
                         TRAP_TO_VMM(fence_i_instruction)
                         :
                         :
                         :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32()
        END_PARAM_SCHEMA()
    END_TRANSLATION(fence_i_instruction);
        BEGIN_PARAM(fence_i_instruction)
            instruction_linear_address
        END_PARAM()
    COMMIT_TRANSLATION(fence_i_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}


static instruction_sub_translator per_funct3_handlers[8];


void
riscv_fence_instructions_translation_entry(struct prefetch_blob * blob,
                                           uint32_t instruction)
{
    struct decoding dec;
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_S);
    ASSERT(per_funct3_handlers[dec.funct3]);
    per_funct3_handlers[dec.funct3](&dec, blob, instruction);
}


__attribute__((constructor)) static void
fence_constructor(void)
{
    memset(per_funct3_handlers, 0x0, sizeof(per_funct3_handlers));
    per_funct3_handlers[0x1] = riscv_fence_i_translator;
}

