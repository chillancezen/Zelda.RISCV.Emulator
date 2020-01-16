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
    uint64_t val = 0;
    struct virtual_machine * vm = hartptr->vmptr;
    void * memory_access_base = vm->bootrom_host_base + (addr - pmr->addr_low);
    switch (access_size)
    {
#define _(size, type)                                                          \
        case size:                                                             \
            val = *(type *)memory_access_base;                                 \
            break
        _(1, uint8_t);
        _(2, uint16_t);
        _(4, uint32_t);
        _(8, uint64_t);
        default:
            __not_reach();
            break;
#undef _
    }
    return val;
}


static void
bootrom_write(uint64_t addr, int access_size, uint64_t value,
                        struct hart * hartptr,
                        struct pm_region_operation * pmr)
{
    struct virtual_machine * vm = hartptr->vmptr;
    void * memory_access_base = vm->bootrom_host_base + (addr - pmr->addr_low);
    switch (access_size)
    {
#define _(size, type)                                                          \
        case size:                                                             \
            *(type *)memory_access_base = (type)value;                         \
            break
        _(1, uint8_t);
        _(2, uint16_t);
        _(4, uint32_t);
        _(8, uint64_t);
        default:
            __not_reach();
            break;
#undef _
    }
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

    // load the rom image to 0x4000.
    const char * rom_image = ini_get(vm->ini_config, "rom", "rom_image");
    const char * pc_on_reset_string = ini_get(vm->ini_config, "rom", "pc_on_reset");
    ASSERT(rom_image);
    ASSERT(pc_on_reset_string);
    uint32_t pc_on_reset = strtol(pc_on_reset_string, NULL, 16);
    ASSERT(!preload_binary_image(vm->bootrom_host_base + pc_on_reset - vm->bootrom_base,
                                 vm->bootrom_size,
                                 rom_image));
}
