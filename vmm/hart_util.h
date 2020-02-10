/*
 * Copyright (c) 2020 Jie Zheng
 *  
 *      This file defines the wrapper to better access hart CSRs and other hart
 *      local resource
 */

#ifndef _HART_UTIL_H
#define _HART_UTIL_H
#include <hart.h>
#include <csr.h>


static inline uint32_t
get_hart_mepc(struct hart * hartptr)
{
    struct csr_entry * csr =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_MEPC];
    return csr->csr_blob;
}

static inline void
adjust_pc_upon_mret(struct hart * hartptr)
{
    uint32_t mepc = get_hart_mepc(hartptr);
    hartptr->pc = mepc;
    log_trace("machine mode returns to:0x%x\n", mepc);
}

static inline void
adjust_mstatus_upon_mret(struct hart * hartptr)
{
    struct csr_entry * csr =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_MSTATUS];
    uint8_t mpp = (csr->csr_blob >> 11) & 0x3;
    ASSERT(mpp == PRIVILEGE_LEVEL_USER ||
           mpp == PRIVILEGE_LEVEL_SUPERVISOR ||
           mpp == PRIVILEGE_LEVEL_MACHINE);
    csr->csr_blob &= ~(3 << 11);
    csr->csr_blob |= PRIVILEGE_LEVEL_USER << 11;
    uint8_t mpie = (csr->csr_blob >> 7) & 0x1;
    csr->csr_blob |= 1 << 7;
    csr->csr_blob &=  ~(1 << 3);
    csr->csr_blob |= mpie << 3;
}

#endif
