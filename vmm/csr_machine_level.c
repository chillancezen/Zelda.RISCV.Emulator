/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <csr.h>



static void
csr_mscratch_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_trace("hart id:%d csr:mscratch write:0x%x\n",hartptr->hart_id, value);
}

static uint32_t
csr_mscratch_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_trace("hart id:%d csr:mscratch read:0x%x\n", hartptr->hart_id,
              csr->csr_blob);
    return csr->csr_blob;
}

static struct csr_registery_entry mscratch_csr_entry ={
    .csr_addr = 0x340,
    .csr_registery = {
        .wpri_mask = 0xffffffff,
        .write = csr_mscratch_write,
        .read = csr_mscratch_read,
        .reset = NULL 
    }
};

__attribute__((constructor)) static void
csr_machine_level_init(void)
{
    register_csr_entry(&mscratch_csr_entry);
}
