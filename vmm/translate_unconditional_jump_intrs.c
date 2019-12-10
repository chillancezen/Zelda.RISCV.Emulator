/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <translation.h>
#include <util.h>
#include <stdio.h>
#include <string.h>

void
riscv_jal_translator(struct prefetch_blob * blob, uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    struct decoding dec;
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_UJ);
    int jump_target = instruction_linear_address +
                      sign_extend32(dec.imm << 1, 20);
    struct program_counter_mapping_item * found_mapping =
        search_translation_item(hartptr, jump_target);

    if (found_mapping) {
        PRECHECK_TRANSLATION_CACHE(jal_instruction_with_target, blob);
        BEGIN_TRANSLATION(jal_instruction_with_target);
        __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl "PIC_PARAM(1)", %%eax;"
                         "movl %%eax, (%%rdx);"
                         "movl "PIC_PARAM(2)", %%eax;"
                         "movl %%eax, (%%r14);"
                         "movl "PIC_PARAM(3)", %%edi;"
                         RESET_ZERO_REGISTER()
                         "addq %%r13, %%rdi;"
                         "jmpq *%%rdi;"
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
        PRECHECK_TRANSLATION_CACHE(jal_instruction_without_target, blob);
        BEGIN_TRANSLATION(jal_instruction_without_target);
        __asm__ volatile("movl "PIC_PARAM(0)", %%edx;"
                         "shl $2, %%edx;"
                         "addq %%r15, %%rdx;"
                         "movl "PIC_PARAM(1)", %%eax;"
                         "movl %%eax, (%%rdx);"
                         "movl "PIC_PARAM(2)", %%eax;"
                         "movl %%eax, (%%r14);"
                         RESET_ZERO_REGISTER()
                         // FIXED: insert instructions to trap to VMM
                         TRAP_TO_VMM(jal_instruction_without_target)
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

    //blob->next_instruction_to_fetch = jump_target;
    blob->is_to_stop = 1;
}

void
riscv_jalr_translator(struct prefetch_blob * blob, uint32_t instruction)
{
    // for riscv jalr instruction, the jump target is calculated only at runtime
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    struct decoding dec;
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_I);
    int32_t signed_offset = sign_extend32(dec.imm, 11);
    PRECHECK_TRANSLATION_CACHE(jalr_instruction, blob);
    BEGIN_TRANSLATION(jalr_instruction);
    __asm__ volatile("movl "PIC_PARAM(2)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl (%%rdx), %%ebx;"
                     "movl "PIC_PARAM(3)", %%eax;"
                     "addl %%eax, %%ebx;" // <=== the jump target
                     "btr $0x0, %%ebx;"
                     "movl "PIC_PARAM(0)", %%edx;"
                     "shl $2, %%edx;"
                     "addq %%r15, %%rdx;"
                     "movl "PIC_PARAM(1)", %%eax;"
                     "movl %%eax, (%%rdx);"
                     "movl %%ebx, (%%r14);" // Update the hart PC
                     RESET_ZERO_REGISTER()
                     TRAP_TO_VMM(jalr_instruction)
                     :
                     :
                     :"memory", "%eax", "%ebx", "%ecx", "%edx");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rd index*/
            PARAM32() /*pc + 4*/
            PARAM32() /*rs1 index*/
            PARAM32() /*imm: signed*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(jalr_instruction);
        BEGIN_PARAM(jalr_instruction)
        dec.rd_index,
        instruction_linear_address + 4,
        dec.rs1_index,
        signed_offset
        END_PARAM()
    COMMIT_TRANSLATION(jalr_instruction, hartptr, instruction_linear_address);
    blob->is_to_stop = 1;
}
