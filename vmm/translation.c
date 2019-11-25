/*
 * Copyright (c) 2019 Jie Zheng
 */

#include <translation.h>
#include <stdio.h>
#include <mmu.h>
#include <string.h>
#include <util.h>


static instruction_translator translators[128];

/*
 * XXX: before control is transfered to guest cache code, the following registers
 * are preserved to filled before switching for special purposes.
 *  r15: the address of the hart's registers group
 *  r14: the address of the hart's pc register
 *  r13: the value of the hart's translation cache base
 *  r12: the hartptr.
 */


void
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
        case ENCODING_TYPE_UJ:
            dec->rd_index = (intrs >> 7) & 0x1f;
            // IMM 20:1
            dec->imm = (intrs >> 21) & 0x3ff; // 10 bits
            dec->imm |= ((intrs >> 20) & 0x1) << 10; // 1 bit
            dec->imm |= ((intrs >> 12) & 0xff) << 11; // 8 bit
            dec->imm |= ((intrs >> 31) & 0x1) << 19; // 1 bit
            break;
        case ENCODING_TYPE_I:
            dec->rd_index = (intrs >> 7) & 0x1f;
            dec->funct3 = (intrs >> 12) & 7;
            dec->rs1_index = (intrs >> 15) & 0x1f;
            // IMM 11:0
            dec->imm = (intrs >> 20) & 0xfff;  
            break;
        case ENCODING_TYPE_S:
            dec->funct3 = (intrs >> 12) & 7;
            dec->rs1_index = (intrs >> 15) & 0x1f;
            dec->rs2_index = (intrs >> 20) & 0x1f;
            dec->imm = (intrs >> 7) & 0x1f;
            dec->imm |= ((intrs >> 25) & 0x7f) << 5;
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

    PRECHECK_TRANSLATION_CACHE(lui_instruction, blob);
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_U);

    BEGIN_TRANSLATION(lui_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%eax;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "shl $12, %%eax;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl %%eax, (%%rdx);"
                     RESET_ZERO_REGISTER()
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

    PRECHECK_TRANSLATION_CACHE(auipc_instruction, blob);
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
                         RESET_ZERO_REGISTER()
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
prefetch_one_instruction(struct prefetch_blob * blob)
{
    struct hart * hartptr = blob->opaque;
    uint32_t instruction = vmread32(hartptr, blob->next_instruction_to_fetch);
    uint8_t opcode = instruction & 0x7f;
    instruction_translator per_category_translator = translators[opcode];
    // NOTE: if assertion takes true, it indicates the instruction is not recognized
    assert(per_category_translator);
    per_category_translator(blob, instruction);
}

extern void * vmm_jumper_begin;
extern void * vmm_jumper_end;
void
prefetch_instructions(struct hart * hartptr)
{
    struct prefetch_blob blob = {
        .next_instruction_to_fetch = hartptr->pc,
        .is_to_stop = 0,
        .is_flushable = 1,
        .opaque = hartptr
    };
    int old_trans_ptr = hartptr->translation_cache_ptr;
    while (1) {
        // See whether the instruction has already been in the translation cache
        // stop translation if so.
        if (search_translation_item(hartptr, blob.next_instruction_to_fetch)) {
            break;
        }
        prefetch_one_instruction(&blob);
        if (blob.is_to_stop) {
            break;
        }
    }
    int new_trans_ptr = hartptr->translation_cache_ptr;
    if (old_trans_ptr != new_trans_ptr) {
        // translation cache updated, append the jumper which directs control
        // to vmm
        uint8_t * jumper_code_begin = (uint8_t *)&vmm_jumper_begin;
        uint8_t * jumper_code_end = (uint8_t *)&vmm_jumper_end;
        int index = 0;
        for (; jumper_code_begin < jumper_code_end;
             jumper_code_begin++, index++) {
            *(uint8_t *)(index + hartptr->translation_cache_ptr +
                         hartptr->translation_cache) = *jumper_code_begin;
        }
    }
}

void
vmresume(struct hart * hartptr)
{
    prefetch_instructions(hartptr);
    // transfer control to guest code by jumping into translation cache
    struct program_counter_mapping_item * ti;
    assert(ti = search_translation_item(hartptr, hartptr->pc));
    __asm__ volatile("movq %%rax, %%r15;"
                     "movq %%rbx, %%r14;"
                     "movq %%rcx, %%r13;"
                     "movq %%rdx, %%r12;"
                     "jmpq *%%rdi;"
                     :
                     :"a"(&hartptr->registers), "b"(&hartptr->pc),
                      "c"(hartptr->translation_cache),
                      "d"(hartptr),
                      "D"(hartptr->translation_cache + ti->tc_offset)
                     :"memory");
}

void
vmexit(struct hart * hartptr)
{
    dump_hart(hartptr);
    vmresume(hartptr);
}

void
vmpanic(struct hart * hartptr)
{
    assert(0);
}

__attribute__((constructor)) void
translation_init(void)
{
    memset(translators, 0x0, sizeof(translators));
    translators[RISCV_OPCODE_LUI] = riscv_lui_translator;
    translators[RISCV_OPCODE_AUIPC] = riscv_auipc_translator;
    translators[RISCV_OPCODE_JAL] = riscv_jal_translator;
    translators[RISCV_OPCODE_JARL] = riscv_jalr_translator;
    translators[RISCV_OPCODE_OP_IMM] = riscv_arithmetic_immediate_instructions_translation_entry;
    translators[RISCV_OPCODE_STORE] = riscv_memory_store_instructions_translation_entry;
    translators[RISCV_OPCODE_LOAD] = riscv_memory_load_instructions_translation_entry;
}
