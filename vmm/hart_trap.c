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
    //uint32_t mpp = hartptr->privilege_level;
    //struct csr_entry * csr_mstatus =
    //    &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_MSTATUS];
    //csr_mstatus->csr_blob &= ~(3 << 11);
    //csr_mstatus->csr_blob |= mpp << 11;
    //hartptr->privilege_level = PRIVILEGE_LEVEL_MACHINE;
    hartptr->status.mpp = hartptr->privilege_level;
    hartptr->privilege_level = PRIVILEGE_LEVEL_MACHINE;


    // MPIE is set to MIE at the time of trap...
    // MIE is set to 0.
    //uint32_t mie = (csr_mstatus->csr_blob >> 3) & 0x1;
    //csr_mstatus->csr_blob &=  ~(1 << 7);
    //csr_mstatus->csr_blob |= mie << 7;
    //csr_mstatus->csr_blob &= ~(1 << 3);
    hartptr->status.mpie = hartptr->status.mie;
    hartptr->status.mie = 0;

    // PC is set to the trap vector.
    struct csr_entry * csr_mtvec =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_MTVEC];
    uint32_t mtvec = csr_mtvec->csr_blob;
    uint32_t trap_mode = mtvec & 0x1;
    if (!trap_mode || !(cause & 0x80000000)) {
        hartptr->pc = mtvec & (~3);
    } else {
        // vectored interrupt delivery
        uint32_t vector = cause & 0x7fffffff;
        hartptr->pc = (mtvec & (~3)) + vector * 4;
    }
}

static void
setup_smode_trap(struct hart * hartptr, uint32_t cause, uint32_t tval)
{
    struct csr_entry * csr_scause =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_SCAUSE];
    csr_scause->csr_blob = cause;
    struct csr_entry * csr_stval =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_STVAL];
    csr_stval->csr_blob = tval;
    struct csr_entry * csr_sepc =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_SEPC];
    csr_sepc->csr_blob = hartptr->pc;

    hartptr->status.spp = hartptr->privilege_level;
    hartptr->privilege_level = PRIVILEGE_LEVEL_SUPERVISOR;
    hartptr->status.spie = hartptr->status.sie;
    hartptr->status.sie = 0;

    struct csr_entry * csr_stvec =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_STVEC];
    uint32_t stvec = csr_stvec->csr_blob;
    uint32_t trap_mode = stvec & 0x1;
    if (!trap_mode || !(cause & 0x80000000)) {
        hartptr->pc = stvec & (~3);
    } else {
        uint32_t vector = cause & 0x7fffffff;
        hartptr->pc = (stvec & (~3)) + vector * 4;
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
    log_trace("trap to privilege:%d cause:0x%08x, tval:0x%08x pc:%08x(current:%d)\n",
              target_privilege_level, cause, tval, hartptr->pc,
              hartptr->privilege_level);
    if (target_privilege_level == PRIVILEGE_LEVEL_MACHINE) {
        setup_mmode_trap(hartptr, cause, tval);
    } else {
        // WE DO NOT SUPPORT USER MODE INTERRUPT
        ASSERT(target_privilege_level == PRIVILEGE_LEVEL_SUPERVISOR);
        setup_smode_trap(hartptr, cause, tval);;
    }

    // XXX: when trap is taken, the addressing manner may chnage, so
    // the translation cache must be flushed.
    flush_translation_cache(hartptr);
    do_trap(hartptr);
}

