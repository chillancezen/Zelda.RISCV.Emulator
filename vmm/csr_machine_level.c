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
    .csr_addr = CSR_ADDRESS_MTVEC,
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
    hartptr->ienable.dword = value;
    log_debug("hart id:%d, csr:mie write 0x:%x\n",
              hartptr->hart_id, hartptr->ienable.dword);
}

static uint32_t
csr_mie_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mie read:0x%x\n",
              hartptr->hart_id, hartptr->ienable.dword);
    return hartptr->ienable.dword;
}

static void
csr_mie_reset(struct hart *hartptr, struct csr_entry * csr)
{
    // disable all interrupts
    hartptr->ienable.dword = 0x0;
}

static struct csr_registery_entry mie_csr_entry = {
    .csr_addr = CSR_ADDRESS_MIE,
    .csr_registery = {
        .wpri_mask = 0x00000aaa,
        .reset = csr_mie_reset,
        .read = csr_mie_read,
        .write = csr_mie_write
    }
};


static void
csr_mip_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    // XXX: MEIP, MTIP and MSIP bits are read-only. any write should never modify
    // these bits.
    // MTIP can only be cleared by writing to MMIO timecmp mtimecmp region.
    // MSIP can only be cleared by writing to MMIO IPI region
    // MEIP is cleared by PLIC.
    uint32_t mip_mask = hartptr->ipending.dword & 0x888;
    hartptr->ipending.dword = (value & (~0x888)) | mip_mask;
    log_debug("hart id:%d, csr:mip write 0x:%x\n",
              hartptr->hart_id, hartptr->ipending.dword);
}

static uint32_t
csr_mip_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mip read:0x%x\n",
              hartptr->hart_id, hartptr->ipending.dword);
    return hartptr->ipending.dword;
}

static void
csr_mip_reset(struct hart *hartptr, struct csr_entry * csr)
{
    // clear all interrupts pending status
    hartptr->ipending.dword = 0x0;
}

static struct csr_registery_entry mip_csr_entry = {
    .csr_addr = CSR_ADDRESS_MIP,
    .csr_registery = {
        .wpri_mask = 0x00000aaa,
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
    .csr_addr = CSR_ADDRESS_MEDELEG,
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
    hartptr->idelegation.dword = value;
    log_debug("hart id:%d, csr:mideleg write 0x:%x\n",
              hartptr->hart_id, hartptr->idelegation.dword);
}

static uint32_t
csr_mideleg_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mideleg read:0x%x\n",
              hartptr->hart_id, hartptr->idelegation.dword);
    return hartptr->idelegation.dword;
}

static void
csr_mideleg_reset(struct hart *hartptr, struct csr_entry * csr)
{
    hartptr->idelegation.dword = 0x0;
}

static struct csr_registery_entry mideleg_csr_entry = {
    .csr_addr = CSR_ADDRESS_MIDELEG,
    .csr_registery = {
        // XXX: only supervisor interrupts can be delegated back to supervisor
        // others are hardwired to ZERO.
        .wpri_mask = 0x00000222,
        .reset = csr_mideleg_reset,
        .read = csr_mideleg_read,
        .write = csr_mideleg_write
    }
};


static void
csr_mstatus_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    hartptr->status.uie = (value >> 0) & 0x1;
    hartptr->status.sie = (value >> 1) & 0x1;
    hartptr->status.mie = (value >> 3) & 0x1;
    hartptr->status.upie = (value >> 4) & 0x1;
    hartptr->status.spie = (value >> 5) & 0x1;
    hartptr->status.mpie = (value >> 7) & 0x1;
    hartptr->status.spp = (value >> 8) & 0x1;
    hartptr->status.mpp = (value >> 11) & 0x3;
    log_debug("hart id:%d, csr:mstatus write 0x:%x\n",
              hartptr->hart_id, value);
}

static uint32_t
csr_mstatus_read(struct hart *hartptr, struct csr_entry *csr)
{
    uint32_t dword = 0;
    dword |= ((uint32_t)hartptr->status.uie) << 0;
    dword |= ((uint32_t)hartptr->status.sie) << 1;
    dword |= ((uint32_t)hartptr->status.mie) << 3;
    dword |= ((uint32_t)hartptr->status.upie) << 4;
    dword |= ((uint32_t)hartptr->status.spie) << 5;
    dword |= ((uint32_t)hartptr->status.mpie) << 7;
    dword |= ((uint32_t)hartptr->status.spp) << 8;
    dword |= ((uint32_t)hartptr->status.mpp) << 11;
    log_debug("hart id:%d, csr:mstatus read:0x%x\n",
              hartptr->hart_id, dword);
    return dword;
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


static void
csr_mcause_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:mcause write 0x:%x\n",
              hartptr->hart_id, csr->csr_blob);
}

static uint32_t
csr_mcause_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mcause read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_mcause_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0x0;
}

static struct csr_registery_entry mcause_csr_entry = {
    .csr_addr = CSR_ADDRESS_MCAUSE,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .reset = csr_mcause_reset,
        .read = csr_mcause_read,
        .write = csr_mcause_write
    }
};

static void
csr_mtval_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:mtval write 0x:%x\n",
              hartptr->hart_id, csr->csr_blob);
}

static uint32_t
csr_mtval_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:mtval read:0x%x\n",
              hartptr->hart_id, csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_mtval_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0x0;
}

static struct csr_registery_entry mtval_csr_entry = {
    .csr_addr = CSR_ADDRESS_MTVAL,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .reset = csr_mtval_reset,
        .read = csr_mtval_read,
        .write = csr_mtval_write
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
    register_csr_entry(&mcause_csr_entry);
    register_csr_entry(&mtval_csr_entry);
}
