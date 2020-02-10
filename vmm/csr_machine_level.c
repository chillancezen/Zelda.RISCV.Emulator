/*
 * Copyright (c) 2019-2020 Jie Zheng
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
    .csr_addr = CSR_ADDRESS_MSCRATCH,
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

static uint32_t
csr_mhartid_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mhartid read:0x%x\n",
              hartptr->hart_id, hartptr->hart_id);
    return hartptr->hart_id;
}


static struct csr_registery_entry mhartid_csr_entry = {
    .csr_addr =0xf14,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .read = csr_mhartid_read
    }
};

static void
csr_mtvec_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:mtvec write 0x:%x\n",
              hartptr->hart_id, csr->csr_blob);
}

static uint32_t
csr_mtvec_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mtvec read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_mtvec_reset(struct hart *hartptr, struct csr_entry * csr)
{
    // All exceptions set pc to BASE. direct
    csr->csr_blob = 0x0;
}

static struct csr_registery_entry mtvec_csr_entry = {
    .csr_addr = 0x305,
    .csr_registery = {
        .wpri_mask = 0xfffffffd,
        .reset = csr_mtvec_reset,
        .read = csr_mtvec_read,
        .write = csr_mtvec_write
    }
};

static void
csr_mcounteren_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:mcounteren write 0x:%x\n",
              hartptr->hart_id, csr->csr_blob);
}

static uint32_t
csr_mcounteren_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mcounteren read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_mcounteren_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0x0;
}

static struct csr_registery_entry mcounteren_csr_entry = {
    .csr_addr = 0x306,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .reset = csr_mcounteren_reset,
        .read = csr_mcounteren_read,
        .write = csr_mcounteren_write
    }
};


static void
csr_mie_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:mie write 0x:%x\n",
              hartptr->hart_id, csr->csr_blob);
}

static uint32_t
csr_mie_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mie read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_mie_reset(struct hart *hartptr, struct csr_entry * csr)
{
    // disable all interrupts
    csr->csr_blob = 0x0;
}

static struct csr_registery_entry mie_csr_entry = {
    .csr_addr = 0x304,
    .csr_registery = {
        .wpri_mask = 0x00000bbb,
        .reset = csr_mie_reset,
        .read = csr_mie_read,
        .write = csr_mie_write
    }
};


static void
csr_mip_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:mip write 0x:%x\n",
              hartptr->hart_id, csr->csr_blob);
}

static uint32_t
csr_mip_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mip read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_mip_reset(struct hart *hartptr, struct csr_entry * csr)
{
    // clear all interrupts pending status
    csr->csr_blob = 0x0;
}

static struct csr_registery_entry mip_csr_entry = {
    .csr_addr = 0x344,
    .csr_registery = {
        .wpri_mask = 0x00000bbb,
        .reset = csr_mip_reset,
        .read = csr_mip_read,
        .write = csr_mip_write
    }
};

static void
csr_medeleg_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:medeleg write 0x:%x\n",
              hartptr->hart_id, csr->csr_blob);
}

static uint32_t
csr_medeleg_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:medeleg read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_medeleg_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0x0;
}

static struct csr_registery_entry medeleg_csr_entry = {
    .csr_addr = 0x302,
    .csr_registery = {
        .wpri_mask = 0xfffff7ff,
        .reset = csr_medeleg_reset,
        .read = csr_medeleg_read,
        .write = csr_medeleg_write
    }
};

static void
csr_mideleg_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:mideleg write 0x:%x\n",
              hartptr->hart_id, csr->csr_blob);
}

static uint32_t
csr_mideleg_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mideleg read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_mideleg_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0x0;
}

static struct csr_registery_entry mideleg_csr_entry = {
    .csr_addr = 0x303,
    .csr_registery = {
        .wpri_mask = 0x00000bbb,
        .reset = csr_mideleg_reset,
        .read = csr_mideleg_read,
        .write = csr_mideleg_write
    }
};


static void
csr_mstatus_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:mstatus write 0x:%x\n",
              hartptr->hart_id, csr->csr_blob);
}

static uint32_t
csr_mstatus_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mstatus read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_mstatus_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0x0;
}

static struct csr_registery_entry mstatus_csr_entry = {
    .csr_addr = CSR_ADDRESS_MSTATUS,
    .csr_registery = {
        // FIXME: some bits are hardwired to Zero, fix them later
        .wpri_mask = 0x000019aa,
        .reset = csr_mstatus_reset,
        .read = csr_mstatus_read,
        .write = csr_mstatus_write
    }
};



static void
csr_mepc_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:mepc write 0x:%x\n",
              hartptr->hart_id, csr->csr_blob);
}

static uint32_t
csr_mepc_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mepc read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_mepc_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0x0;
}

static struct csr_registery_entry mepc_csr_entry = {
    .csr_addr = CSR_ADDRESS_MEPC,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .reset = csr_mepc_reset,
        .read = csr_mepc_read,
        .write = csr_mepc_write
    }
};

__attribute__((constructor)) static void
csr_machine_level_init(void)
{
    register_csr_entry(&misa_csr_entry);
    register_csr_entry(&mscratch_csr_entry);
    register_csr_entry(&mvendorid_csr_entry);
    register_csr_entry(&mhartid_csr_entry);
    register_csr_entry(&mtvec_csr_entry);
    register_csr_entry(&mcounteren_csr_entry);
    register_csr_entry(&mie_csr_entry);
    register_csr_entry(&mip_csr_entry); 
    register_csr_entry(&medeleg_csr_entry);
    register_csr_entry(&mideleg_csr_entry);
    register_csr_entry(&mstatus_csr_entry);
    register_csr_entry(&mepc_csr_entry);
}
