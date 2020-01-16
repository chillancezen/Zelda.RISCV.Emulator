/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <vm.h>
#include <string.h>
#include <physical_memory.h>
#include <stdlib.h>
#include <util.h>
#include <pm_region.h>
#include <fcntl.h>
#include <unistd.h>

static uint64_t
main_memory_direct_read(uint64_t addr, int access_size,
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
main_memory_direct_write(uint64_t addr, int access_size, uint64_t value,
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

static void
memory_init(struct virtual_machine * vm)
{
    // after the main memory is allocated. register it as the physical memory region
    // in the global list. it's backed as main memory.
    struct pm_region_operation main_memory_pmr = {
        .addr_low = vm->main_mem_base,
        .addr_high = vm->main_mem_base + vm->main_mem_size,
        .pmr_read = main_memory_direct_read,
        .pmr_write = main_memory_direct_write,
        .pmr_desc = "main.memory"
    };
    register_pm_region_operation(&main_memory_pmr);
}

static void
device_init(struct virtual_machine * vm)
{
    uart_init();
}

void
virtual_machine_init(struct virtual_machine * vm,
                     const struct virtual_machine_spec * spec)
{
    int idx = 0;
    struct hart * hart_ptr;
    memset(vm, 0x0, sizeof(struct virtual_machine));
    // nothing can precede ini configuration
    vm->ini_config = spec->ini_config;

    bootrom_init(vm);

    vm->main_mem_base = spec->main_mem_base;
    vm->main_mem_size = MEGA(spec->main_mem_size_in_mega);
    vm->main_mem_host_base = preallocate_physical_memory(vm->main_mem_size);
    ASSERT(vm->main_mem_host_base);
    

    vm->nr_harts = spec->nr_harts;
    vm->boot_hart = spec->boot_cpu;
    vm->harts = aligned_alloc(64, vm->nr_harts * sizeof(struct hart));
    ASSERT(vm->harts);

    for (idx = 0; idx < vm->nr_harts; idx++) {
        hart_ptr = hart_by_id(vm, idx);
        hart_init(hart_ptr, idx);
        hart_ptr->pc = 0x4000;
        hart_ptr->vmptr = vm;
    }

    ASSERT(!preload_binary_image(vm->main_mem_host_base + spec->image_load_base - vm->main_mem_base,
                                 vm->main_mem_size, spec->image_path));
    memory_init(vm);
    device_init(vm);

    dump_memory_regions();

    // build the device tree
    build_device_tree(&vm->fdt);
}

