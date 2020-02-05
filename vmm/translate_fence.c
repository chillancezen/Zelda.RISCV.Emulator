/*
 * Copyright (c) 2019-2020 Jie Zheng
 */

#include <translation.h>
#include <util.h>

// FENCE.I instruction order instruction cache and data cache, any guest JIT
// system should issue a FENCE.I instruction at its end of self-modifying code
// LIKE JAVA.
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
                         SAVE_GUEST_CONTEXT_SWITCH_REGS()
                         "call *%%rax;"
                         RESTORE_GUEST_CONTEXT_SWITCH_REGS()
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
    // Stop translation because after the fence.i instruction, the 
    // translation cache will be flushed
    blob->is_to_stop = 1;
}


// FENCE instruction is to order memeroy Read/Write and Device Input/Ouput
// in our hart implementation(emulation), all instructions are exactly in order.
static void
riscv_fence_translator(struct decoding * dec, struct prefetch_blob * blob,
                    uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(fence_instruction, blob);
    BEGIN_TRANSLATION(fence_instruction);
        __asm__ volatile(PROCEED_TO_NEXT_INSTRUCTION()
                         END_INSTRUCTION(fence_instruction)
                         :
                         :
                         :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32()
        END_PARAM_SCHEMA()
    END_TRANSLATION(fence_instruction);
        BEGIN_PARAM(fence_instruction)
            instruction_linear_address
        END_PARAM()
    COMMIT_TRANSLATION(fence_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static instruction_sub_translator per_funct3_handlers[8];


void
riscv_fence_instructions_translation_entry(struct prefetch_blob * blob,
                                           uint32_t instruction)
{
    struct decoding dec;
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_S);
    if (!per_funct3_handlers[dec.funct3]) {
        log_fatal("no handler for fence_instructions:%d @0x%x\n",
                  dec.funct3, blob->next_instruction_to_fetch);
    }
    ASSERT(per_funct3_handlers[dec.funct3]);
    per_funct3_handlers[dec.funct3](&dec, blob, instruction);
}


__attribute__((constructor)) static void
fence_constructor(void)
{
    memset(per_funct3_handlers, 0x0, sizeof(per_funct3_handlers));
    per_funct3_handlers[0x0] = riscv_fence_translator;
    per_funct3_handlers[0x1] = riscv_fence_i_translator;
}

