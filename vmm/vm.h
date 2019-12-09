/*
 * Copyright (c) 2019 Jie Zheng
 */

#ifndef _VM_H
#define _VM_H
#include <stdint.h>
#include <hart.h>
#include <physical_memory.h>
#include <util.h>

struct virtual_machine {

    int nr_harts;
    struct hart * harts;
    
    int64_t pmem_size;
    void * pmem_base;
};

__attribute__((always_inline))
static inline struct hart *
hart_by_id(struct virtual_machine * vm, int hart_id)
{
    ASSERT(hart_id >= 0 && hart_id < vm->nr_harts);
    return &vm->harts[hart_id];
}


#define MEGA(nr_mega) (1024 * 1024 * (nr_mega))

struct virtual_machine_spec {
    int nr_harts;
    int pmem_size_in_mega;
    char * image_path;
    uint32_t entry_point;
    uint32_t image_load_offset;
};

void
virtual_machine_init(struct virtual_machine * vm,
                     const struct virtual_machine_spec * spec);

void
uart_init(void);

#endif
