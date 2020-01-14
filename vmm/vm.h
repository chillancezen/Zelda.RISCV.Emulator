/*
 * Copyright (c) 2019 Jie Zheng
 */

#ifndef _VM_H
#define _VM_H
#include <stdint.h>
#include <hart.h>
#include <physical_memory.h>
#include <util.h>
#include <fdt.h>

struct virtual_machine {

    int nr_harts;
    int boot_hart;
    struct hart * harts;
    
    int64_t main_mem_size;
    int64_t main_mem_base;
    void * main_mem_host_base;


    // the build buffer of FDT
    struct fdt_build_blob fdt;
};

__attribute__((always_inline))
static inline struct hart *
hart_by_id(struct virtual_machine * vm, int hart_id)
{
    ASSERT(hart_id >= 0 && hart_id < vm->nr_harts);
    return &vm->harts[hart_id];
}


#define MEGA(nr_mega) (1024 * 1024 * (nr_mega))

#define IMAGE_TYPE_BINARY 0x1
#define IMAGE_TYPE_ELF 0x2

struct virtual_machine_spec {
    // [CPU]
    int nr_harts;
    int boot_cpu; // default is 0th
    
    // [MEM]
    uint32_t main_mem_base;
    int main_mem_size_in_mega;

    // [IMAGE]
    int image_type;
    char * image_path;
    uint32_t entry_point;
    uint32_t image_load_base;
};

void
virtual_machine_init(struct virtual_machine * vm,
                     const struct virtual_machine_spec * spec);

void
uart_init(void);

#endif
