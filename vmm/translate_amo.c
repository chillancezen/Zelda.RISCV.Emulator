/*
 * Copyright (c) 2020 Jie Zheng
 */

#include <translation.h>
#include <util.h>
#include <mmu.h>

static void
amoadd_slowpath(struct hart * hartptr, uint8_t rs1_index, uint8_t rs2_index,
                uint8_t rd_index)
{
    uint32_t * regs = (uint32_t *)&hartptr->registers;
    // MUST make a copy of rs1 and rs2. because if rd equals any of them, things
    // getting wrong.
    uint32_t rs1 = regs[rs1_index];
    uint32_t rs2 = regs[rs2_index];
    regs[rd_index]= mmu_read32_aligned(hartptr, rs1);
    mmu_write32_aligned(hartptr, rs1, regs[rd_index] + rs2);
}


static void
amoswap_slowpath(struct hart * hartptr, uint8_t rs1_index, uint8_t rs2_index,
                uint8_t rd_index)
{
    uint32_t * regs = (uint32_t *)&hartptr->registers;
    uint32_t rs1 = regs[rs1_index];
    uint32_t rs2 = regs[rs2_index];
    regs[rd_index]= mmu_read32_aligned(hartptr, rs1);
    mmu_write32_aligned(hartptr, rs1, rs2);
}

struct address_reservation_item {
    uint32_t addr;
    uint32_t valid:1;
    uint32_t hartid:8;
};
// FIXME: GUARD ALL THE CRITICAL RESOURCE IF WE ARE GOING TO REALIZE SMP
#define MAX_RESERVATION_ITEMS   16
static struct address_reservation_item reservations[MAX_RESERVATION_ITEMS];
static int next_pointer = 0;

static void
lr_slowpath(struct hart * hartptr, uint8_t rs1_index, uint8_t rs2_index,
                uint8_t rd_index)
{
    uint32_t * regs = (uint32_t *)&hartptr->registers;
    uint32_t rs1 = regs[rs1_index];
    regs[rd_index]= mmu_read32_aligned(hartptr, rs1);
    // MAKE A RESERVATION FOR ADDRESS RS1
    int idx = 0;
    for (idx = 0; idx < MAX_RESERVATION_ITEMS; idx++) {
        if (!reservations[idx].valid) {
            next_pointer = idx;
            break;
        }
    }
    reservations[next_pointer].addr = rs1;
    reservations[next_pointer].valid = 1;
    reservations[next_pointer].hartid = hartptr->hart_id;
    next_pointer = (next_pointer + 1) % MAX_RESERVATION_ITEMS;
    log_trace("lr.w address reservation:0x%08x\n", rs1);
}

static void
sc_slowpath(struct hart * hartptr, uint8_t rs1_index, uint8_t rs2_index,
                uint8_t rd_index)
{
    uint32_t * regs = (uint32_t *)&hartptr->registers;
    uint32_t rs1 = regs[rs1_index];
    uint32_t rs2 = regs[rs2_index];
    int idx = 0;
    for (; idx < MAX_RESERVATION_ITEMS; idx++) {
        if (reservations[idx].valid &&
            reservations[idx].addr == rs1 &&
            reservations[idx].hartid == hartptr->hart_id) {
            break;
        } 
    }
    if (idx < MAX_RESERVATION_ITEMS) {
        // FOUND A VALID RESERVATION ITEM
        regs[rd_index] = 0x0;
        mmu_write32_aligned(hartptr, rs1, rs2);
        // CLEAR ADDRESS RESERVATION
        reservations[idx].valid = 0;
    } else {
        regs[rd_index] = 0x1;
    }
}

static void
amoxor_slowpath(struct hart * hartptr, uint8_t rs1_index, uint8_t rs2_index,
                uint8_t rd_index)
{
    uint32_t * regs = (uint32_t *)&hartptr->registers;
    uint32_t rs1 = regs[rs1_index];
    uint32_t rs2 = regs[rs2_index];
    regs[rd_index]= mmu_read32_aligned(hartptr, rs1);
    mmu_write32_aligned(hartptr, rs1, regs[rd_index] ^ rs2);
}

static void
amoor_slowpath(struct hart * hartptr, uint8_t rs1_index, uint8_t rs2_index,
                uint8_t rd_index)
{
    uint32_t * regs = (uint32_t *)&hartptr->registers;
    uint32_t rs1 = regs[rs1_index];
    uint32_t rs2 = regs[rs2_index];
    regs[rd_index]= mmu_read32_aligned(hartptr, rs1);
    mmu_write32_aligned(hartptr, rs1, regs[rd_index] | rs2);
}

static void
amoand_slowpath(struct hart * hartptr, uint8_t rs1_index, uint8_t rs2_index,
                uint8_t rd_index)
{
    uint32_t * regs = (uint32_t *)&hartptr->registers;
    uint32_t rs1 = regs[rs1_index];
    uint32_t rs2 = regs[rs2_index];
    regs[rd_index]= mmu_read32_aligned(hartptr, rs1);
    mmu_write32_aligned(hartptr, rs1, regs[rd_index] & rs2);
}

void
amo_instruction_slowpath(struct hart * hartptr, uint8_t rs1_index,
                         uint8_t rs2_index, uint8_t rd_index, uint32_t funct5)
{
#define _(funct5, func)                                                        \
        case funct5:                                                           \
            func(hartptr, rs1_index, rs2_index, rd_index);                     \
            break;
    switch(funct5)
    {
        _(0x0, amoadd_slowpath);
        _(0x1, amoswap_slowpath);
        _(0x2, lr_slowpath);
        _(0x3, sc_slowpath);
        _(0x4, amoxor_slowpath);
        _(0x8, amoor_slowpath);
        _(0xc, amoand_slowpath);
        default:
            __not_reach();
            break;
    }
#undef _
}
static void
riscv_amo_translator(struct decoding * dec, struct prefetch_blob * blob,
                        uint32_t instruction)
{
    uint32_t instruction_linear_address = blob->next_instruction_to_fetch;
    struct hart * hartptr = (struct hart *)blob->opaque;
    PRECHECK_TRANSLATION_CACHE(amo_instruction, blob);
    BEGIN_TRANSLATION(amo_instruction);
    __asm__ volatile(
                    // RDI: hartptr
                    // RSI: rs1_index,
                    // RDX: rs2_index,
                    // RCX: rd_index
                    // R8: funct5
                     "movq %%r12, %%rdi;"
                     "movl "PIC_PARAM(0)", %%esi;"
                     "movl "PIC_PARAM(1)", %%edx;"
                     "movl "PIC_PARAM(2)", %%ecx;"
                     "movl "PIC_PARAM(3)", %%r8d;"
                     "movq $amo_instruction_slowpath, %%rax;"
                     SAVE_GUEST_CONTEXT_SWITCH_REGS()
                     "call *%%rax;"
                     RESTORE_GUEST_CONTEXT_SWITCH_REGS()
                     RESET_ZERO_REGISTER()
                     PROCEED_TO_NEXT_INSTRUCTION()
                     END_INSTRUCTION(amo_instruction)
                     :
                     :
                     :"memory");
        BEGIN_PARAM_SCHEMA()
            PARAM32() /*rs1 index*/
            PARAM32() /*rs2 index*/
            PARAM32() /*rd index*/
            PARAM32() /*funct5*/
        END_PARAM_SCHEMA()
    END_TRANSLATION(amo_instruction);
        BEGIN_PARAM(amo_instruction)
            dec->rs1_index,
            dec->rs2_index,
            dec->rd_index,
            dec->funct7 >> 2
        END_PARAM()
    COMMIT_TRANSLATION(amo_instruction, hartptr, instruction_linear_address);
    blob->next_instruction_to_fetch += 4;
}

static instruction_sub_translator per_funct5_handlers[32];

void
riscv_amo_instructions_translation_entry(struct prefetch_blob * blob,
                                         uint32_t instruction)
{
    struct decoding dec;
    instruction_decoding_per_type(&dec, instruction, ENCODING_TYPE_R);
    uint8_t funct5 = dec.funct7 >> 2;
    if (!per_funct5_handlers[funct5]) {
        printf("instruction_linear_address:%x\n", blob->next_instruction_to_fetch);
    }
    ASSERT(per_funct5_handlers[funct5]);
    per_funct5_handlers[funct5](&dec, blob, instruction);
}


__attribute__((constructor)) static void
amo_constructor(void)
{
    memset(reservations, 0x0, sizeof(reservations));
    memset(per_funct5_handlers, 0x0, sizeof(per_funct5_handlers));
    per_funct5_handlers[0] = riscv_amo_translator; //amoadd.w
    per_funct5_handlers[0x1] = riscv_amo_translator; // amoswap.w
    per_funct5_handlers[0x2] = riscv_amo_translator; // lr.w
    per_funct5_handlers[0x3] = riscv_amo_translator; // sc.w
    per_funct5_handlers[0x4] = riscv_amo_translator; //amoxor.w
    per_funct5_handlers[0x8] = riscv_amo_translator; // amoor.w
    per_funct5_handlers[0xc] = riscv_amo_translator; // amoand.w
}
