/*
 * Copyright (c) 2019 Jie Zheng
 */

#include <translation.h>
#include <stdio.h>
#include <mmu.h>
#include <string.h>
#include <util.h>


//typedef int (*instruction_translator)(uint32_t, uint32_t, void *);
typedef void (*instruction_translator)(struct prefetch_blob * blob, uint32_t);
static instruction_translator translators[128];

/*
 * XXX: before control is translated to guest cache code, the following registers
 * are preserved for special purposes.
 *  r15: the address of the hart's registers group
 *  r14: the address of the hart's pc register
 *  r13: the address of the hart's translation cache base
 */


static void
instruction_decoding_per_type(struct decoding * dec, 
                              uint32_t intrs,
                              enum INSTRUCTION_ENCODING_TYPE encoding_type)
{
    memset(dec, 0x0, sizeof(struct decoding));
    dec->opcode = intrs & 0x7f;
    dec->instr_encoding_type = encoding_type;
    switch(encoding_type)
    {
        case ENCODING_TYPE_U:
            dec->rd_index = (intrs >> 7) & 0x1f;
            // IMM 31:12
            dec->imm = (intrs >> 12) & 0xfffff;
            break;
        case ENCODING_TYPE_J:
            dec->rd_index = (intrs >> 7) & 0x1f;
            // IMM 20:1
            dec->imm = (intrs >> 21) & 0x3ff; // 10 bits
            dec->imm |= ((intrs >> 20) & 0x1) << 10; // 1 bit
            dec->imm |= ((intrs >> 12) & 0xff) << 11; // 8 bit
            dec->imm |= ((intrs >> 31) & 0x1) << 19; // 1 bit
            break;
        default:
            assert(0);
            break;
    }
}

static void
riscv_lui_translator(struct prefetch_blob * blob, uint32_t instruction)
{
    
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    struct decoding dec;

    if (TRANSLATION_SIZE(lui_instruction) > unoccupied_cache_size(hartptr)) {
        blob->is_to_stop = 1;
        return;
    }
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_U);

    BEGIN_TRANSLATION(lui_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%eax;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $12, %%eax;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(lui_instruction)
                     :
                     :
                     :"memory", "%r15", "%rax", "%rdx");

        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm*/
            PARAM32() /*rd*/
        END_PARAM_SCHEMA()
    
    END_TRANSLATION(lui_instruction);

        BEGIN_PARAM(lui_instruction)
            dec.imm,
            dec.rd_index
        END_PARAM()

    COMMIT_TRANSLATION(lui_instruction, hartptr, instruction_linear_address);

    blob->next_instruction_to_fetch += 4;
}

static void
riscv_auipc_translator(struct prefetch_blob * blob, uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    struct decoding dec;
    if (TRANSLATION_SIZE(auipc_instruction) > unoccupied_cache_size(hartptr)) {
        blob->is_to_stop = 1;
        return;
    }
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_U);

    BEGIN_TRANSLATION(auipc_instruction);
        __asm__ volatile("movl "PIC_PARAM(1)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl "PIC_PARAM(0)", %%eax;"
                         "shl $12, %%eax;"
                         "movl "PIC_PARAM(2)", %%ecx;"
                         "addl %%ecx, %%eax;"
                         "movl %%eax, (%%rdx);"
                         PROCEED_TO_NEXT_INSTRUCTION()
                         END_INSTRUCTION(auipc_instruction)
                         :
                         :
                         :"memory", "%rax", "%rcx", "%rdx", "%r15");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*imm*/
            PARAM32() /*rd*/
            PARAM32() /*pc*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(auipc_instruction);
        BEGIN_PARAM(auipc_instruction)
            dec.imm,
            dec.rd_index,
            instruction_linear_address
        END_PARAM()
    COMMIT_TRANSLATION(auipc_instruction, hartptr, instruction_linear_address);

    blob->next_instruction_to_fetch += 4;
}

static void
riscv_jal_translator(struct prefetch_blob * blob, uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    struct decoding dec;
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_J);
    int jump_target = instruction_linear_address +
                      sign_extend32(dec.imm << 1, 20);
    struct program_counter_mapping_item * found_mapping =
        search_translation_item(hartptr, jump_target);

    if (found_mapping) {
        BEGIN_TRANSLATION(jal_instruction_with_target);
        __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl "PIC_PARAM(1)", %%eax;"
                         "movl %%eax, (%%rdx);"
                         "movl "PIC_PARAM(2)", %%eax;"
                         "movl %%eax, (%%r14);"
                         "movl "PIC_PARAM(3)", %%edi;"
                         "addq %%r13, %%rdi;"
                         "jmpq *%%rdi;"
                         END_INSTRUCTION(jal_instruction_with_target)
                         :
                         :
                         :"memory", "%rax", "%rdx", "%rdi");
            BEGIN_PARAM_SCHEMA()
                PARAM32() /*rd*/
                PARAM32() /*pc + 4*/
                PARAM32() /*unconditional jump_target of guest*/
                PARAM32() /*unconditional jump_target in translation cache*/
            END_PARAM_SCHEMA()
        END_TRANSLATION(jal_instruction_with_target);
            BEGIN_PARAM(jal_instruction_with_target)
                dec.rd_index,
                instruction_linear_address + 4,
                jump_target,
                found_mapping->tc_offset
            END_PARAM()
        COMMIT_TRANSLATION(jal_instruction_with_target, hartptr,
                           instruction_linear_address);
    } else {
        BEGIN_TRANSLATION(jal_instruction_without_target);
        __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl "PIC_PARAM(1)", %%eax;"
                         "movl %%eax, (%%rdx);"
                         "movl "PIC_PARAM(2)", %%eax;"
                         "movl %%eax, (%%r14);"
                         END_INSTRUCTION(jal_instruction_without_target)
                         :
                         :
                         :"memory", "%rax", "%rdx");
            BEGIN_PARAM_SCHEMA()
                PARAM32() /*rd*/
                PARAM32() /*pc + 4*/
                PARAM32() /*unconditional jump_target of guest*/
            END_PARAM_SCHEMA()
        END_TRANSLATION(jal_instruction_without_target);
            BEGIN_PARAM(jal_instruction_without_target)
                dec.rd_index,
                instruction_linear_address + 4,
                jump_target
            END_PARAM()
        COMMIT_TRANSLATION(jal_instruction_without_target, hartptr,
                           instruction_linear_address); 
    }

    blob->next_instruction_to_fetch = jump_target;
    blob->is_to_stop = 1;
}

static void
prefetch_one_instruction(struct prefetch_blob * blob)
{
    struct hart * hartptr = blob->opaque;
    uint32_t instruction = vmread32(hartptr->vmptr,
                                    blob->next_instruction_to_fetch);
    uint8_t opcode = instruction & 0x7f;
    instruction_translator per_category_translator = translators[opcode];
    // NOTE: if assertion takes true, it indicates the instruction is not recognized
    assert(per_category_translator);
    per_category_translator(blob, instruction);
}

void
prefetch_instructions(struct hart * hartptr)
{
    struct prefetch_blob blob = {
        .next_instruction_to_fetch = hartptr->pc,
        .is_to_stop = 0,
        .opaque = hartptr
    };
    while (1) {
        prefetch_one_instruction(&blob);
        if (blob.is_to_stop) {
            break;
        }
    }
}

void
vmresume(struct hart * hartptr)
{
    prefetch_instructions(hartptr);
}


__attribute__((constructor)) void
translation_init(void)
{
    memset(translators, 0x0, sizeof(translators));
    translators[RISCV_OPCODE_LUI] = riscv_lui_translator;
    translators[RISCV_OPCODE_AUIPC] = riscv_auipc_translator;
    translators[RISCV_OPCODE_JAL] = riscv_jal_translator;
}
