/*
 * Copyright (c) 2020 Jie Zheng
 */
#include <vm.h>
#include <stdlib.h>
#include <log.h>
#include <pm_region.h>

static uint64_t
clint_read(uint64_t addr, int access_size, struct hart * hartptr,
             struct pm_region_operation * pmr)
{

    __not_reach();
    return 0;
}



static void
clint_write(uint64_t addr, int access_size, uint64_t value,
                        struct hart * hartptr,
                        struct pm_region_operation * pmr)
{
    //__not_reach();
}


void
clint_init(struct virtual_machine * vm)
{
    const char * clint_base_string = ini_get(vm->ini_config, "cpu", "clint_base");
    const char * clint_size_string = ini_get(vm->ini_config, "cpu", "clint_size");
    ASSERT(clint_base_string);
    ASSERT(clint_size_string);
    uint32_t clint_base = strtol(clint_base_string, NULL, 16);
    uint32_t clint_size = strtol(clint_size_string, NULL, 16);
    struct pm_region_operation clint_pmr = {
        .addr_low = clint_base,
        .addr_high = clint_base + clint_size,
        .pmr_read = clint_read,
        .pmr_write = clint_write,
        .pmr_desc = "clint.mmio"
    };
    register_pm_region_operation(&clint_pmr);
}

