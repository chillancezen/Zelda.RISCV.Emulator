/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <vm.h>
#include <string.h>
#include <physical_memory.h>
#include <assert.h>
#include <stdlib.h>
#include <util.h>

void
virtual_machine_init(struct virtual_machine * vm,
                     const struct virtual_machine_spec * spec)
{
    int idx = 0;
    struct hart * hart_ptr;
    memset(vm, 0x0, sizeof(struct virtual_machine));
    vm->pmem_size = MEGA(spec->pmem_size_in_mega);
    vm->pmem_base = preallocate_physical_memory(vm->pmem_size);
    assert(vm->pmem_base);
    
    vm->nr_harts = spec->nr_harts;
    vm->harts = aligned_alloc(64, vm->nr_harts * sizeof(struct hart));
    assert(vm->harts);

    for (idx = 0; idx < vm->nr_harts; idx++) {
        hart_ptr = hart_by_id(vm, idx);
        hart_init(hart_ptr, idx);
        hart_ptr->pc = spec->entry_point;
        hart_ptr->vmptr = vm;
    }

    assert(!preload_image(vm->pmem_base + spec->image_load_offset,
                          vm->pmem_size, spec->image_path));
}
