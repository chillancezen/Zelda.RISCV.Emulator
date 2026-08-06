/*
 * Copyright (c) 2020 Jie Zheng
 */

#include <csr.h>
#include <clint.h>

/*
 * time/timeh are architecturally a read-only mirror of the CLINT's mtime, so
 * they must come from the same virtual clock the timer compares against.
 * Reading the host TSC here instead would give the guest a clock running at
 * host speed while its timer interrupts arrive at emulated speed, and every
 * derived value - delay loops, printk timestamps, scheduler accounting -
 * would be wrong by the ratio between the two.
 */
static void
csr_time_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
}

static uint32_t
csr_time_read(struct hart *hartptr, struct csr_entry *csr)
{
    hartptr->tsc = clint_get_time();
    return (uint32_t)hartptr->tsc;
}

static struct csr_registery_entry time_csr_entry = {
    .csr_addr = CSR_ADDRESS_TIME,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .read = csr_time_read,
        .write = csr_time_write
    }
};

static void
csr_timeh_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
}

/*
 * Reads the high half of the value latched by the most recent low-half read,
 * which is what the standard rv32 read-high/read-low/re-read-high loop
 * expects.
 */
static uint32_t
csr_timeh_read(struct hart *hartptr, struct csr_entry *csr)
{
    return (uint32_t)(clint_get_time() >> 32);
}

static struct csr_registery_entry timeh_csr_entry = {
    .csr_addr = CSR_ADDRESS_TIMEH,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .read = csr_timeh_read,
        .write = csr_timeh_write
    }
};

/*
 * Counters the guest is allowed to read but which we do not model separately:
 * cycle and instret track the same virtual clock as time. Anything else in
 * this table exists only so that touching it does not panic the vmm.
 */
static uint32_t
csr_cycle_read(struct hart *hartptr, struct csr_entry *csr)
{
    return (uint32_t)clint_get_time();
}

static uint32_t
csr_cycleh_read(struct hart *hartptr, struct csr_entry *csr)
{
    return (uint32_t)(clint_get_time() >> 32);
}

static void
csr_scratch_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
    csr->csr_blob = value;
}

static uint32_t
csr_scratch_read(struct hart *hartptr, struct csr_entry *csr)
{
    return csr->csr_blob;
}

#define CSR_ADDRESS_CYCLE       0xC00
#define CSR_ADDRESS_INSTRET     0xC02
#define CSR_ADDRESS_CYCLEH      0xC80
#define CSR_ADDRESS_INSTRETH    0xC82
#define CSR_ADDRESS_MCYCLE      0xB00
#define CSR_ADDRESS_MINSTRET    0xB02
#define CSR_ADDRESS_MCYCLEH     0xB80
#define CSR_ADDRESS_MINSTRETH   0xB82
#define CSR_ADDRESS_MARCHID     0xF12
#define CSR_ADDRESS_MIMPID      0xF13
#define CSR_ADDRESS_MSTATUSH    0x310
#define CSR_ADDRESS_MENVCFG     0x30A
#define CSR_ADDRESS_MENVCFGH    0x31A
#define CSR_ADDRESS_SENVCFG     0x10A

static struct csr_registery_entry counter_csr_entries[] = {
    {.csr_addr = CSR_ADDRESS_CYCLE,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_cycle_read}},
    {.csr_addr = CSR_ADDRESS_INSTRET,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_cycle_read}},
    {.csr_addr = CSR_ADDRESS_CYCLEH,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_cycleh_read}},
    {.csr_addr = CSR_ADDRESS_INSTRETH,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_cycleh_read}},
    {.csr_addr = CSR_ADDRESS_MCYCLE,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_cycle_read,
                       .write = csr_time_write}},
    {.csr_addr = CSR_ADDRESS_MINSTRET,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_cycle_read,
                       .write = csr_time_write}},
    {.csr_addr = CSR_ADDRESS_MCYCLEH,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_cycleh_read,
                       .write = csr_time_write}},
    {.csr_addr = CSR_ADDRESS_MINSTRETH,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_cycleh_read,
                       .write = csr_time_write}},
    // Identification registers: zero is the architecturally defined
    // "not implemented" answer and is what SBI reports upwards.
    {.csr_addr = CSR_ADDRESS_MARCHID,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_scratch_read}},
    {.csr_addr = CSR_ADDRESS_MIMPID,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_scratch_read}},
    // Configuration registers we accept writes to but do not act on: none of
    // the features they gate are implemented.
    {.csr_addr = CSR_ADDRESS_MSTATUSH,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_scratch_read,
                       .write = csr_scratch_write}},
    {.csr_addr = CSR_ADDRESS_MENVCFG,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_scratch_read,
                       .write = csr_scratch_write}},
    {.csr_addr = CSR_ADDRESS_MENVCFGH,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_scratch_read,
                       .write = csr_scratch_write}},
    {.csr_addr = CSR_ADDRESS_SENVCFG,
     .csr_registery = {.wpri_mask = WPRI_MASK_ALL, .read = csr_scratch_read,
                       .write = csr_scratch_write}},
};

__attribute__((constructor)) static void
csr_misc_init(void)
{
    int idx = 0;
    register_csr_entry(&time_csr_entry);
    register_csr_entry(&timeh_csr_entry);
    for (idx = 0;
         idx < (int)(sizeof(counter_csr_entries)/sizeof(counter_csr_entries[0]));
         idx++) {
        register_csr_entry(&counter_csr_entries[idx]);
    }
}
