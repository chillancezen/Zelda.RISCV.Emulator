/*
 * Copyright (c) 2019 Jie Zheng
 *  
 *      This file extends supervisor-level instructions translation
 */
#include <csr.h>
#include <translation.h>
#include <util.h>

void
riscv_generic_csr_callback(struct hart * hartptr, uint64_t instruction)
{
    ASSERT(hartptr->hart_magic == HART_MAGIC_WORD);
    ASSERT((instruction & 0x7f) ==  RISCV_OPCODE_SUPERVISOR_LEVEL);
    struct decoding dec;
    instruction_decoding_per_type(&dec, (uint32_t)instruction, ENCODING_TYPE_I);
    struct csr_entry * csr = &((struct csr_entry *)hartptr->csrs_base)[dec.imm & 0xfff];
    // FIXME: swith it on
    //ASSERT(csr->is_valid);
    //ASSERT(csr->read);
    //ASSERT(csr->write);
    switch (dec.funct3)
    {
        case 0x01:
        case 0x05:
            // CSRRW and CSRRWI
            {
                // store rs1 reg in case rs1_index == rd_index
                uint32_t rs1_reg = HART_REG(hartptr, dec.rs1_index);
                if (dec.rd_index) {
                    // if RD register is zero(x0), don't read the csr to avoid read side effect
                    uint32_t value_to_read = 0;
                    if (csr->read) {
                        value_to_read = csr->read(hartptr, csr);
                    }
                    value_to_read &= csr->wpri_mask;
                    HART_REG(hartptr, dec.rd_index) = value_to_read;
                }
                uint32_t value_to_write = dec.funct3 == 0x01 ? rs1_reg: dec.rs1_index;
                value_to_write &= csr->wpri_mask;
                if (csr->write) {
                    csr->write(hartptr, csr, value_to_write);
                }
            }
            break;
        case 0x2:
        case 0x6:
            // csrrs and csrrsi
            {
                uint32_t rs1_reg = HART_REG(hartptr, dec.rs1_index);
                uint32_t value_to_read = 0;
                if (csr->read) {
                    value_to_read = csr->read(hartptr, csr);
                }
                value_to_read &= csr->wpri_mask;
                HART_REG(hartptr, dec.rd_index) = value_to_read;

                uint32_t value_to_set = 0;
                uint8_t proceed_to_write = 0;
                if (dec.funct3 == 0x2 && dec.rs1_index) {
                    value_to_set = rs1_reg;
                    proceed_to_write = 1;
                } else if(dec.funct3 == 0x6 && dec.rs1_index) {
                    value_to_set = dec.rs1_index & 0x1f;
                    proceed_to_write = 1;
                }
                if (proceed_to_write && csr->write) {
                    // if RS1 is zero or rs1(imm)'s value is zero, don't write csr
                    csr->write(hartptr, csr, (value_to_read | value_to_set) &
                                             csr->wpri_mask);
                }
            }
            break;
        case 0x3:
        case 0x7:
            // csrrc and csrrci
            {
                uint32_t rs1_reg = HART_REG(hartptr, dec.rs1_index);
                uint32_t value_to_read = 0;
                if (csr->read) {
                    value_to_read = csr->read(hartptr, csr);
                }
                value_to_read &= csr->wpri_mask;
                HART_REG(hartptr, dec.rd_index) = value_to_read;

                uint32_t value_to_clear = 0;
                uint8_t proceed_to_write = 0;

                if (dec.funct3 == 0x3 && dec.rs1_index) {
                    value_to_clear = rs1_reg;
                    proceed_to_write = 1;
                } else if(dec.funct3 == 0x7 && dec.rs1_index){
                    value_to_clear = dec.rs1_index & 0x1f;
                    proceed_to_write = 1;
                }
                if (proceed_to_write && csr->write) {
                    csr->write(hartptr, csr, value_to_read & ~value_to_clear &
                                             csr->wpri_mask);
                }
            }
            break;
        default:
            __not_reach();
            break;
    }
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
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;"
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
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

