/*
 * Copyright (c) 2020 Jie Zheng
 */
#include <hart_trap.h>
#include <csr.h>

static void
setup_mmode_trap(struct hart * hartptr, uint32_t cause, uint32_t tval)
{
    struct csr_entry * csr_mcause =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_MCAUSE];
    csr_mcause->csr_blob = cause;
    struct csr_entry * csr_mtval =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_MTVAL];
    csr_mtval->csr_blob = tval;
    struct csr_entry * csr_mepc =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_MEPC];
    csr_mepc->csr_blob = hartptr->pc; // PC is kept in MEPC
    
    // MPP is set to the privilege level at the time of trap...
    // and current privilege level is set to machine mode.
    uint32_t mpp = hartptr->privilege_level;
    struct csr_entry * csr_mstatus =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_MSTATUS];
    csr_mstatus->csr_blob &= ~(3 << 11);
    csr_mstatus->csr_blob |= mpp << 11;
    hartptr->privilege_level = PRIVILEGE_LEVEL_MACHINE;

    // MPIE is set to MIE at the time of trap...
    // MIE is set to 0.
    uint32_t mie = (csr_mstatus->csr_blob >> 3) & 0x1;
    csr_mstatus->csr_blob &=  ~(1 << 7);
    csr_mstatus->csr_blob |= mie << 7;
    csr_mstatus->csr_blob &= ~(1 << 3);

    // PC is set to the trap vector.
    struct csr_entry * csr_mtvec =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_MTVEC];
    uint32_t mtvec = csr_mtvec->csr_blob;
    uint8_t trap_mode = mtvec & 0x1;
    if (!trap_mode || !(cause & 0x80000000)) {
        hartptr->pc = mtvec & (~3);
    } else {
        // vectored interrupt delivery
        uint32_t vector = cause & 0x7fffffff;
        hartptr->pc = (mtvec & (~3)) + vector * 4;
    }
}

extern void vmm_entry_point(void);

static void
do_trap(struct hart * hartptr)
{
    __asm__ volatile("movq %%rax, %%r15;"
                     "movq %%rbx, %%r14;"
                     "movq %%rcx, %%r13;"
                     "movq %%rdx, %%r12;"
                     "jmpq *%%rdi;"
                     :
                     :"a"(&hartptr->registers), "b"(&hartptr->pc),
                      "c"(hartptr->translation_cache),
                      "d"(hartptr),
                      "D"(vmm_entry_point)
                     :"memory");
}

/*
 * raise_trap_raw
 *
 *      The function is to raise a trap synchronously. 
 *      the wrapper(Interrupt/Exception) should themselves make decision whether
 *      it's the right time to deliver a trap.
 */
void
raise_trap_raw(struct hart * hartptr, uint8_t target_privilege_level,
               uint32_t cause, uint32_t tval)
{
    if (target_privilege_level == PRIVILEGE_LEVEL_MACHINE) {
        setup_mmode_trap(hartptr, cause, tval);
    } else {
        __not_reach();
    }

    do_trap(hartptr);
}

