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
#include <ini.h>

struct virtual_machine {

    int nr_harts;
    int boot_hart;
    struct hart * harts;
    
    // main memory
    int64_t main_mem_size;
    int64_t main_mem_base;
    void * main_mem_host_base;
    
    // bootrom
    int64_t bootrom_size;
    int64_t bootrom_base;
    void * bootrom_host_base;

    // the build buffer of FDT
    struct fdt_build_blob fdt;

    // the ini conifguration
    ini_t * ini_config;
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

void
virtual_machine_init(struct virtual_machine * vm, ini_t *);

void
bootrom_init(struct virtual_machine * vm);

void
ram_init(struct virtual_machine * vm);

void
uart_init(void);

#endif
