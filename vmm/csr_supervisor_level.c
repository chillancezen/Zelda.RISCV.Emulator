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




static void
csr_satp_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:satp write 0x:%x\n",
              hartptr->hart_id, csr->csr_blob);
}

static uint32_t
csr_satp_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:satp read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_satp_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0x0;
}

static struct csr_registery_entry satp_csr_entry = {
    .csr_addr = 0x180,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .reset = csr_satp_reset,
        .read = csr_satp_read,
        .write = csr_satp_write
    }
};



static void
csr_sie_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    hartptr->ienable.bits.ssi = (value >> 1) & 0x1;
    hartptr->ienable.bits.sti = (value >> 5) & 0x1;
    hartptr->ienable.bits.sei = (value >> 9) & 0x1;
    log_debug("hart id:%d, csr:sie write 0x:%x\n",
              hartptr->hart_id, value);
}

static uint32_t
csr_sie_read(struct hart *hartptr, struct csr_entry *csr)
{
    uint32_t blob = 0;
    blob |= (uint32_t)(hartptr->ienable.bits.usi) << 0;
    blob |= (uint32_t)(hartptr->ienable.bits.ssi) << 1;
    blob |= (uint32_t)(hartptr->ienable.bits.uti) << 4;
    blob |= (uint32_t)(hartptr->ienable.bits.sti) << 5;
    blob |= (uint32_t)(hartptr->ienable.bits.uei) << 8;
    blob |= (uint32_t)(hartptr->ienable.bits.sei) << 9;
    log_debug("hart id:%d, csr:sie read:0x%x\n",
              hartptr->hart_id, blob);
    return blob;
}


static struct csr_registery_entry sie_csr_entry = {
    .csr_addr = CSR_ADDRESS_SIE,
    .csr_registery = {
        .wpri_mask = 0x00000222,
        .read = csr_sie_read,
        .write = csr_sie_write
    }
};


static void
csr_sip_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    // ONLY SSIP is writable.
    hartptr->ipending.bits.ssi = (value >> 1) & 0x1;
    log_debug("hart id:%d, csr:sip write 0x:%x\n",
              hartptr->hart_id, value);
}

static uint32_t
csr_sip_read(struct hart *hartptr, struct csr_entry *csr)
{
    uint32_t blob = 0;
    blob |= (uint32_t)(hartptr->ipending.bits.ssi) << 1;
    blob |= (uint32_t)(hartptr->ipending.bits.sti) << 5;
    blob |= (uint32_t)(hartptr->ipending.bits.sei) << 9;
    log_debug("hart id:%d, csr:sip read:0x%x\n",
              hartptr->hart_id, blob);
    return blob;
}


static struct csr_registery_entry sip_csr_entry = {
    .csr_addr = CSR_ADDRESS_SIP,
    .csr_registery = {
        .wpri_mask = 0x00000222,
        .read = csr_sip_read,
        .write = csr_sip_write
    }
};



static void
csr_sstatus_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    hartptr->status.uie = (value >> 0) & 0x1;
    hartptr->status.sie = (value >> 1) & 0x1;
    hartptr->status.upie = (value >> 4) & 0x1;
    hartptr->status.spie = (value >> 5) & 0x1;
    hartptr->status.spp = (value >> 8) & 0x1;
    log_debug("hart id:%d, csr:sstatus write 0x:%x\n",
              hartptr->hart_id, value);
}

static uint32_t
csr_sstatus_read(struct hart *hartptr, struct csr_entry *csr)
{
    uint32_t blob = 0;
    blob |= (uint32_t)(hartptr->status.uie) << 0;
    blob |= (uint32_t)(hartptr->status.sie) << 1;
    blob |= (uint32_t)(hartptr->status.upie) << 4;
    blob |= (uint32_t)(hartptr->status.spie) << 5;
    blob |= (uint32_t)(hartptr->status.spp) << 8;
    log_debug("hart id:%d, csr:sstatus read:0x%x\n",
              hartptr->hart_id, blob);
    return blob;
}


static struct csr_registery_entry sstatus_csr_entry = {
    .csr_addr = CSR_ADDRESS_SSTATUS,
    .csr_registery = {
        .wpri_mask = 0x00000133,
        .read = csr_sstatus_read,
        .write = csr_sstatus_write
    }
};

__attribute__((constructor)) static void
csr_supervisor_level_init(void)
{
    register_csr_entry(&scounteren_csr_entry);
    register_csr_entry(&satp_csr_entry);
    register_csr_entry(&sie_csr_entry);
    register_csr_entry(&sip_csr_entry);
    register_csr_entry(&sstatus_csr_entry);
}

