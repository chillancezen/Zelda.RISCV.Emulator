/*
 * Copyright (c) 2020 Jie Zheng
 */
#include <csr.h>

#define PMPADDR_BASE 0x3b0
#define NR_PMPADDR 16

#define PMPCFG_BASE 0x3a0
#define NR_PMPCFG 4

static void
csr_pmpaddr_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:pmpaddr%d write 0x:%x\n",
              hartptr->hart_id,
              csr->csr_addr - PMPADDR_BASE,
              csr->csr_blob);
}

static uint32_t
csr_pmpaddr_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:pmpaddr%d read:0x%x\n",
              hartptr->hart_id,
              csr->csr_addr - PMPADDR_BASE,
              csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_pmpaddr_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0x0;
}

#define DEFINE_PMPADDR_ENTRY(idx)                                              \
static struct csr_registery_entry pmpaddr##idx##_csr_entry = {                 \
    .csr_addr = PMPADDR_BASE + idx,                                            \
    .csr_registery = {                                                         \
        .wpri_mask = WPRI_MASK_ALL,                                            \
        .reset = csr_pmpaddr_reset,                                            \
        .read = csr_pmpaddr_read,                                              \
        .write = csr_pmpaddr_write                                             \
    }                                                                          \
}

DEFINE_PMPADDR_ENTRY(0);
DEFINE_PMPADDR_ENTRY(1);
DEFINE_PMPADDR_ENTRY(2);
DEFINE_PMPADDR_ENTRY(3);
DEFINE_PMPADDR_ENTRY(4);
DEFINE_PMPADDR_ENTRY(5);
DEFINE_PMPADDR_ENTRY(6);
DEFINE_PMPADDR_ENTRY(7);
DEFINE_PMPADDR_ENTRY(8);
DEFINE_PMPADDR_ENTRY(9);
DEFINE_PMPADDR_ENTRY(10);
DEFINE_PMPADDR_ENTRY(11);
DEFINE_PMPADDR_ENTRY(12);
DEFINE_PMPADDR_ENTRY(13);
DEFINE_PMPADDR_ENTRY(14);
DEFINE_PMPADDR_ENTRY(15);

static void
csr_pmpcfg_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
    log_debug("hart id:%d, csr:pmpcfg%d write 0x:%x\n",
              hartptr->hart_id,
              csr->csr_addr - PMPCFG_BASE,
              csr->csr_blob);
}

static uint32_t
csr_pmpcfg_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:pmpcfg%d read:0x%x\n",
              hartptr->hart_id,
              csr->csr_addr - PMPCFG_BASE,
              csr->csr_blob);
    return csr->csr_blob;
}

static void
csr_pmpcfg_reset(struct hart *hartptr, struct csr_entry * csr)
{
    csr->csr_blob = 0x0;
}

#define DEFINE_PMPCFG_ENTRY(idx)                                               \
static struct csr_registery_entry pmpcfg##idx##_csr_entry = {                  \
    .csr_addr = PMPCFG_BASE + idx,                                             \
    .csr_registery = {                                                         \
        .wpri_mask = WPRI_MASK_ALL,                                            \
        .reset = csr_pmpcfg_reset,                                             \
        .read = csr_pmpcfg_read,                                               \
        .write = csr_pmpcfg_write                                              \
    }                                                                          \
}

DEFINE_PMPCFG_ENTRY(0);
DEFINE_PMPCFG_ENTRY(1);
DEFINE_PMPCFG_ENTRY(2);
DEFINE_PMPCFG_ENTRY(3);

__attribute__((constructor)) static void
csr_machine_level_init(void)
{
#define _(idx) register_csr_entry(&pmpaddr##idx##_csr_entry)
    _(0);
    _(1);
    _(2);
    _(3);
    _(4);
    _(5);
    _(6);
    _(7);
    _(8);
    _(9);
    _(10);
    _(11);
    _(12);
    _(13);
    _(14);
    _(15);
#undef _

#define _(idx) register_csr_entry(&pmpcfg##idx##_csr_entry)
    _(0);
    _(1);
    _(2);
    _(3);
#undef _
}
