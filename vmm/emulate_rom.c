/*
 * Copyright (c) 2020 Jie Zheng
 */
#include <vm.h>
#include <stdlib.h>
#include <log.h>
#include <pm_region.h>

static uint64_t
bootrom_read(uint64_t addr, int access_size, struct hart * hartptr,
             struct pm_region_operation * pmr)
{

    return 0;
}


static void
bootrom_write(uint64_t addr, int access_size, uint64_t value,
                        struct hart * hartptr,
                        struct pm_region_operation * pmr)
{

}

void
bootrom_init(struct virtual_machine * vm)
{
    // Preallocate the memory area of BOOTROM
    const char * rom_start_string = ini_get(vm->ini_config, "rom", "rom_start");
    const char * rom_size_string = ini_get(vm->ini_config, "rom", "rom_size");
    ASSERT(rom_start_string);
    ASSERT(rom_size_string);
    vm->bootrom_base = strtol(rom_start_string, NULL, 16);
    vm->bootrom_size = strtol(rom_size_string, NULL, 16);
    vm->bootrom_host_base = preallocate_physical_memory(vm->bootrom_size);
    ASSERT(vm->bootrom_host_base);

    // register physical memory region
    struct pm_region_operation bootrom_pmr = {
        .addr_low = vm->bootrom_base,
        .addr_high = vm->bootrom_base + vm->bootrom_size,
        .pmr_read = bootrom_read,
        .pmr_write = bootrom_write,
        .pmr_desc = "bootrom"
    };
    register_pm_region_operation(&bootrom_pmr);
}
