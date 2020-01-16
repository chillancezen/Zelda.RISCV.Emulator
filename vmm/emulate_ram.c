/*
 * Copyright (c) 2020 Jie Zheng
 */

#include <vm.h>
#include <pm_region.h>

static uint64_t
ram_read(uint64_t addr, int access_size,
                        struct hart * hartptr,
                        struct pm_region_operation * pmr)
{
    uint64_t val = 0;
    struct virtual_machine * vm = hartptr->vmptr;
    void * memory_access_base = vm->main_mem_host_base + (addr - pmr->addr_low);
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
ram_write(uint64_t addr, int access_size, uint64_t value,
                        struct hart * hartptr,
                        struct pm_region_operation * pmr)
{
    struct virtual_machine * vm = hartptr->vmptr;
    void * memory_access_base = vm->main_mem_host_base + (addr - pmr->addr_low);
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
ram_init(struct virtual_machine * vm)
{
    // preallocate physical memory for emulated RAM
    const char * ram_start_string = ini_get(vm->ini_config, "mem", "main_memory_start");
    const char * ram_size_string = ini_get(vm->ini_config, "mem", "main_memory_size_in_mega");
    ASSERT(ram_start_string);
    ASSERT(ram_size_string);
    uint32_t ram_start = strtol(ram_start_string, NULL, 16);
    uint32_t ram_size = strtol(ram_size_string, NULL, 10);
    vm->main_mem_base = ram_start;
    vm->main_mem_size = MEGA(ram_size);
    vm->main_mem_host_base = preallocate_physical_memory(vm->main_mem_size);
    ASSERT(vm->main_mem_host_base);

    // register the physical memory region for RAM
    struct pm_region_operation main_memory_pmr = {
        .addr_low = vm->main_mem_base,
        .addr_high = vm->main_mem_base + vm->main_mem_size,
        .pmr_read = ram_read,
        .pmr_write = ram_write,
        .pmr_desc = "main.memory"
    };
    register_pm_region_operation(&main_memory_pmr);

    // load the image(maybe a Linux kernel) into ram here
    // FIXME: we need a more sophisticated way to load image later. rigt now we
    // just load a binary image.
    const char * image_path = ini_get(vm->ini_config, "image", "path");
    const char * load_base_string = ini_get(vm->ini_config, "image", "load_base");
    ASSERT(image_path);
    ASSERT(load_base_string);
    uint32_t load_base = strtol(load_base_string, NULL, 16);
    ASSERT(!preload_binary_image(vm->main_mem_host_base + load_base - vm->main_mem_base,
                                 vm->main_mem_size, image_path));
}
