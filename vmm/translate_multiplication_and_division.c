/*
 * Copyright (c) 2019 Jie Zheng
 */

#include <translation.h>
#include <util.h>
#include <string.h>
#include <hart_exception.h>

static instruction_sub_translator per_funct3_handlers[8];

static void
riscv_mul_translator(struct decoding * dec,
                     struct prefetch_blob * blob,
                     uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(mul_instruction, blob);
    BEGIN_TRANSLATION(mul_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%edx;"
                     "movl "PIC_PARAM(1)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%eax;"
                     "imul %%edx;" // EAX is reserved
                     "movl "PIC_PARAM(2)", %%edi;"
                     "shl $0x2, %%edi;"
                     "addq %%r15, %%rdi;"
                     "movl %%eax, (%%rdi);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(mul_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(mul_instruction);
        BEGIN_PARAM(mul_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(mul_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}


static void
riscv_mulh_translator(struct decoding * dec,
                      struct prefetch_blob * blob,
                      uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(mulh_instruction, blob);
    BEGIN_TRANSLATION(mulh_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%edx;"
                     "movl "PIC_PARAM(1)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%eax;"
                     "imul %%edx;" // EDX is reserved
                     "movl "PIC_PARAM(2)", %%edi;"
                     "shl $0x2, %%edi;"
                     "addq %%r15, %%rdi;"
                     "movl %%edx, (%%rdi);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(mulh_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(mulh_instruction);
        BEGIN_PARAM(mulh_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(mulh_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_mulhu_translator(struct decoding * dec,
                       struct prefetch_blob * blob,
                       uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(mulhu_instruction, blob);
    BEGIN_TRANSLATION(mulhu_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%edx;"
                     "movl "PIC_PARAM(1)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%eax;"
                     "mul %%edx;" // EDX is reserved
                     "movl "PIC_PARAM(2)", %%edi;"
                     "shl $0x2, %%edi;"
                     "addq %%r15, %%rdi;"
                     "movl %%edx, (%%rdi);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(mulhu_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(mulhu_instruction);
        BEGIN_PARAM(mulhu_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(mulhu_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_mulhsu_translator(struct decoding * dec,
                        struct prefetch_blob * blob,
                        uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(mulhsu_instruction, blob);
    BEGIN_TRANSLATION(mulhsu_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%eax;"
                     "movsxd %%eax, %%rax;"// signed RS1
                     "movl "PIC_PARAM(1)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%edx;"// unsigned RS2
                     "imul %%rdx;" // RAX[63:32] bits => RD
                     "shr $32, %%rax;"
                     "movl "PIC_PARAM(2)", %%edi;"
                     "shl $0x2, %%edi;"
                     "addq %%r15, %%rdi;"
                     "movl %%eax, (%%rdi);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(mulhsu_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index: signed*/
            PARAM32() /*rs2 index: unsigned*/
            PARAM32() /*rd index: high XLEN-bit*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(mulhsu_instruction);
        BEGIN_PARAM(mulhsu_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(mulhsu_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}


// FIXME: the overfolow case for risc-v div and rem(divu and remu is all right):
// li a0, -1 << 31
// li a1, -1
// div a2, a0, a1
// rdx:rax = 0xffffffff:0x80000000
// rbx = 0xffffffff
// VMM: Floating point exception
static void
riscv_div_translator(struct decoding * dec,
                     struct prefetch_blob * blob,
                     uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(div_instruction, blob);
    BEGIN_TRANSLATION(div_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%eax;"
                     "movsxd %%eax, %%rdx;"
                     "shr $32, %%rdx;" // EDX:EAX: signed RS1
                     "movl "PIC_PARAM(1)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%ebx;"// RS2 XXX: divisor must be non-zero.
                     "movl %%ebx, %%r8d;"
                     "cmpl $0x0, %%r8d;"
                     "jnz 1f;"
                     "movl $0xffffffff, %%eax;"
                     "jmp 2f;"
                     "1:idiv %%ebx;" // eax<= quotient, edx<= remainder
                     "2:movl "PIC_PARAM(2)", %%edi;"
                     "shl $0x2, %%edi;"
                     "addq %%r15, %%rdi;"
                     "movl %%eax, (%%rdi);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(div_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index: signed*/
            PARAM32() /*rs2 index: unsigned*/
            PARAM32() /*rd index: high XLEN-bit*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(div_instruction);
        BEGIN_PARAM(div_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(div_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_rem_translator(struct decoding * dec,
                     struct prefetch_blob * blob,
                     uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(rem_instruction, blob);
    BEGIN_TRANSLATION(rem_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%eax;"
                     "movsxd %%eax, %%rdx;"
                     "shr $32, %%rdx;" // EDX:EAX: signed RS1
                     "movl "PIC_PARAM(1)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%ebx;"// RS2 XXX: divisor must be non-zero.
                     "idiv %%ebx;" // eax<= quotient, edx<= remainder
                     "movl "PIC_PARAM(2)", %%edi;"
                     "shl $0x2, %%edi;"
                     "addq %%r15, %%rdi;"
                     "movl %%edx, (%%rdi);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(rem_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index: signed*/
            PARAM32() /*rs2 index: unsigned*/
            PARAM32() /*rd index: high XLEN-bit*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(rem_instruction);
        BEGIN_PARAM(rem_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(rem_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_divu_translator(struct decoding * dec,
                      struct prefetch_blob * blob,
                      uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(divu_instruction, blob);
    BEGIN_TRANSLATION(divu_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%eax;"
                     "xorl %%edx, %%edx;" // EDX:EAX: unsigned RS1
                     "movl "PIC_PARAM(1)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%ebx;"// RS2 XXX: divisor must be non-zero.
                     "movl %%ebx, %%r8d;"
                     "cmpl $0x0, %%r8d;"
                     "jnz 1f;"
                     "movl $0xffffffff, %%eax;"
                     "jmp 2f;"
                     "1:div %%ebx;" // eax<= quotient, edx<= remainder
                     "2:movl "PIC_PARAM(2)", %%edi;"
                     "shl $0x2, %%edi;"
                     "addq %%r15, %%rdi;"
                     "movl %%eax, (%%rdi);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(divu_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index: signed*/
            PARAM32() /*rs2 index: unsigned*/
            PARAM32() /*rd index: high XLEN-bit*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(divu_instruction);
        BEGIN_PARAM(divu_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(divu_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static void
riscv_remu_translator(struct decoding * dec,
                      struct prefetch_blob * blob,
                      uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(remu_instruction, blob);
    BEGIN_TRANSLATION(remu_instruction);
    __asm__ volatile("movl "PIC_PARAM(0)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%eax;"
                     "xorl %%edx, %%edx;" // EDX:EAX: unsigned RS1
                     "movl "PIC_PARAM(1)", %%esi;"
                     "shl $0x2, %%esi;"
                     "addq %%r15, %%rsi;"
                     "movl (%%rsi), %%ebx;"// RS2 XXX: divisor must be non-zero.
                     "div %%ebx;" // eax<= quotient, edx<= remainder
                     "movl "PIC_PARAM(2)", %%edi;"
                     "shl $0x2, %%edi;"
                     "addq %%r15, %%rdi;"
                     "movl %%edx, (%%rdi);"
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(remu_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index: signed*/
            PARAM32() /*rs2 index: unsigned*/
            PARAM32() /*rd index: high XLEN-bit*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(remu_instruction);
        BEGIN_PARAM(remu_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index
        END_PARAM()
    COMMIT_TRANSLATION(remu_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

void
riscv_mul_div_instructions_translation_entry(struct decoding * dec,
                                             struct prefetch_blob * blob,
                                             uint32_t instruction)
{
    ASSERT(per_funct3_handlers[dec->funct3]);
    per_funct3_handlers[dec->funct3](dec, blob, instruction);
}



__attribute__((constructor)) static void
mul_and_div_constructor(void)
{
    memset(per_funct3_handlers, 0x0, sizeof(per_funct3_handlers));
    per_funct3_handlers[0x0] = riscv_mul_translator;
    per_funct3_handlers[0x1] = riscv_mulh_translator;
    per_funct3_handlers[0x3] = riscv_mulhu_translator;
    per_funct3_handlers[0x2] = riscv_mulhsu_translator;
    per_funct3_handlers[0x4] = riscv_div_translator;
    per_funct3_handlers[0x5] = riscv_divu_translator;
    per_funct3_handlers[0x6] = riscv_rem_translator;
    per_funct3_handlers[0x7] = riscv_remu_translator;
}
#include <fenv.h>
__attribute__((constructor)) static void
vmm_floating_exception_trap_init(void)
{
    feclearexcept(FE_ALL_EXCEPT);
}
