/*
 * Copyright (c) 2020 Jie Zheng
 */

#include <csr.h>

static uint64_t
get_host_tsc(void)
{
    uint32_t eax = 0;
    uint32_t edx = 0;
    __asm__ volatile("rdtsc;"
                     :"=a"(eax), "=d"(edx)
                     :
                     :"memory");
    return ((uint64_t)eax) | (((uint64_t)edx) << 32);
}

static void
csr_time_write(struct hart *hartptr, struct csr_entry * csr, uint32_t value)
{
}

static uint32_t
csr_time_read(struct hart *hartptr, struct csr_entry *csr)
{
    hartptr->tsc = get_host_tsc();
    log_debug("hart id:%d, csr:time read:0x%x\n",
              hartptr->hart_id, (uint32_t)hartptr->tsc);
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

static uint32_t
csr_timeh_read(struct hart *hartptr, struct csr_entry *csr)
{
    log_debug("hart id:%d, csr:timeh read:0x%x\n",
              hartptr->hart_id, (uint32_t)(hartptr->tsc >> 32));
    return (uint32_t)(hartptr->tsc >> 32);
}

static struct csr_registery_entry timeh_csr_entry = {
    .csr_addr = CSR_ADDRESS_TIMEH,
    .csr_registery = {
        .wpri_mask = WPRI_MASK_ALL,
        .read = csr_timeh_read,
        .write = csr_timeh_write
    }
};

__attribute__((constructor)) static void
csr_misc_init(void)
{
    register_csr_entry(&time_csr_entry);
    register_csr_entry(&timeh_csr_entry);
}

