/*
 * Copyright (c) 2020 Jie Zheng
 */
#include <csr.h>


static void
csr_scounteren_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:scounteren write 0x:%x\n",
              hartptr->hart_id, csr->csr_blob);
}

static uint32_t
csr_scounteren_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:scounteren read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_scounteren_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0x0;
}

static struct csr_registery_entry scounteren_csr_entry = {
    .csr_addr = 0x106,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .reset = csr_scounteren_reset,
        .read = csr_scounteren_read,
        .write = csr_scounteren_write
    }
};

__attribute__((constructor)) static void
csr_supervisor_level_init(void)
{
    register_csr_entry(&scounteren_csr_entry);
}

