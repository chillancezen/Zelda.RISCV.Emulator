/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <vm.h>
#include <string.h>
#include <physical_memory.h>
#include <stdlib.h>
#include <util.h>
#include <pm_region.h>


static uint64_t
main_memory_direct_read(uint64_t addr, int access_size,
                        struct hart * hartptr,
                        struct pm_region_operation * pmr)
{
    uint64_t val = 0;
    struct virtual_machine * vm = hartptr->vmptr;
    void * memory_access_base = vm->pmem_base + (addr - pmr->addr_low);
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
    void * memory_access_base = vm->pmem_base + (addr - pmr->addr_low);
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
        .addr_low = 0,
        .addr_high = vm->pmem_size,
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
    vm->pmem_size = MEGA(spec->pmem_size_in_mega);
    vm->pmem_base = preallocate_physical_memory(vm->pmem_size);
    ASSERT(vm->pmem_base);
    
    vm->nr_harts = spec->nr_harts;
    vm->harts = aligned_alloc(64, vm->nr_harts * sizeof(struct hart));
    ASSERT(vm->harts);

    for (idx = 0; idx < vm->nr_harts; idx++) {
        hart_ptr = hart_by_id(vm, idx);
        hart_init(hart_ptr, idx);
        hart_ptr->pc = spec->entry_point;
        hart_ptr->vmptr = vm;
    }

    ASSERT(!preload_image(vm->pmem_base + spec->image_load_offset,
                          vm->pmem_size, spec->image_path));
    memory_init(vm);
    device_init(vm);
}
