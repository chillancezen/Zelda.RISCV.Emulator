/*
 * Copyright (c) 2019 Jie Zheng
 */

#include <translation.h>
#include <stdio.h>
#include <mmu.h>
#include <string.h>



typedef int (*instruction_translator)(uint32_t, uint32_t, void *);

static instruction_translator translators[128];



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
            dec->imm = (intrs >> 12) & 0xfffff;
            break;
        default:
            assert(0);
            break;
    }
}

static int
riscv_lui_translator(uint32_t instruction_linear_address,
                     uint32_t instruction,
                     void * opaque)
{

    struct hart * hartptr = (struct hart *)opaque;
    struct decoding dec;

    if (TRANSLATION_SIZE(lui_instruction) > unoccupied_cache_size(hartptr)) {
        return -1;
    }
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_U);

    BEGIN_TRANSLATION(lui_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%eax;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $12, %%eax;"
                     "shl $2, %%edx;"
                     "addq %%rbx, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     END_INSTRUCTION(lui_instruction)
                     :
                     :
                     :"memory", "%rbx", "%rax", "%rdx");

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
    
    return 0;
}

static int
riscv_auipc_translator(uint32_t instruction_linear_address,
                       uint32_t instruction,
                       void * opaque)
{
    struct hart * hartptr = (struct hart *)opaque;
    struct decoding dec;
    if (TRANSLATION_SIZE(auipc_instruction) > unoccupied_cache_size(hartptr)) {
        return -1;
    }
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_U);

    BEGIN_TRANSLATION(auipc_instruction);
        __asm__ volatile("movl "PIC_PARAM(1)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%rbx, %%rdx;"
                         "movl "PIC_PARAM(0)", %%eax;"
                         "shl $12, %%eax;"
                         "movl "PIC_PARAM(2)", %%ecx;"
                         "addl %%ecx, %%eax;"
                         "movl %%eax, (%%rdx);"
                         END_INSTRUCTION(auipc_instruction)
                         :
                         :
                         :"memory", "%rax", "%rcx", "%rdx");
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

    return 0;
}

static int
translate_one_instruction(struct hart * hartptr,
                          uint32_t instruction_linear_address,
                          uint32_t instruction)
{
    uint8_t opcode = instruction & 0x7f;
    instruction_translator per_category_translator = translators[opcode];
    // NOTE: if assertion takes true, it reveals the instruction is not recognized
    assert(per_category_translator);
    return per_category_translator(instruction_linear_address, instruction,
                                   hartptr);
}

void
prefetch_instructions(struct hart * hartptr)
{
    while (1) {
        uint32_t instr = vmread32(hartptr->vmptr, hartptr->pc);
        int rc = translate_one_instruction(hartptr, hartptr->pc, instr);
        if (rc) {
            break;
        }
        hartptr->pc += 4;
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
}
