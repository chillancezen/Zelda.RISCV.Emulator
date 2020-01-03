/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <csr.h>



static void
csr_mscratch_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d csr:mscratch write:0x%x\n",hartptr->hart_id, value);
}

static uint32_t
csr_mscratch_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d csr:mscratch read:0x%x\n", hartptr->hart_id,
              csr->csr_blob);
    return csr->csr_blob;
}

static struct csr_registery_entry mscratch_csr_entry = {
    .csr_addr = 0x340,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .write = csr_mscratch_write,
        .read = csr_mscratch_read,
        .reset = NULL 
    }
};


static void
csr_misa_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    log_debug("hart id:%d, csr:misa write ignored\n",
              hartptr->hart_id);
}

static uint32_t
csr_misa_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:misa read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_misa_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0;
    csr->csr_blob |= 1 << 0; // atomic extension
    csr->csr_blob |= 1 << 8; // RV32I base ISA
    csr->csr_blob |= 1 << 12; // integer multiply and divide extension
    csr->csr_blob |= 1 << 18; // supervisor mode implemented
    csr->csr_blob |= 1 << 20; // user mode implementedb
    csr->csr_blob |= 1 << 30; // MXLEN
}

static struct csr_registery_entry misa_csr_entry = {
    .csr_addr = 0x301,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .write = csr_misa_write,
        .read = csr_misa_read,
        .reset = csr_misa_reset
    }
};

static void
csr_mvendorid_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0x60a;
}

static uint32_t
csr_mvendorid_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mvendorid read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}


static struct csr_registery_entry mvendorid_csr_entry = {
    .csr_addr =0xf11,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .read = csr_mvendorid_read,
        .reset = csr_mvendorid_reset
    }
};

__attribute__((constructor)) static void
csr_machine_level_init(void)
{
    register_csr_entry(&misa_csr_entry);
    register_csr_entry(&mscratch_csr_entry);
    register_csr_entry(&mvendorid_csr_entry);
}
