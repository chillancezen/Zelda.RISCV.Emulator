/*
 * Copyright (c) 2019 Jie Zheng
 */
#ifndef _PM_REGION_H
#define _PM_REGION_H
#include <stdint.h>
#include <string.h>
#include <hart.h>

#define MAX_NR_PM_REGIONS 256

struct pm_region_operation;

typedef uint64_t pm_region_read_callback(uint64_t addr, int access_size,
                                         struct hart * hartptr,
                                         struct pm_region_operation * pmr);

typedef void pm_region_write_callback(uint64_t addr, int access_size, uint64_t value,
                                      struct hart * hartptr,
                                      struct pm_region_operation * pmr);

struct pm_region_operation {
    uint32_t addr_low;
    uint32_t addr_high;
    pm_region_read_callback * pmr_read;
    pm_region_write_callback * pmr_write;
    char * pmr_desc;
};

void
register_pm_region_operation(const struct pm_region_operation * pro);

struct pm_region_operation *
search_pm_region_callback(uint64_t guest_pa);

#endif
