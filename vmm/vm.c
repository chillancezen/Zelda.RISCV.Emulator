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
#include <debug.h>
#include <mmu_tlb.h>

static void
device_init(struct virtual_machine * vm)
{
    clint_init(vm);
    uart_init();
}

static void
hart_tlb_init(struct hart * hartptr)
{
    hartptr->itlb = aligned_alloc(64, sizeof(struct tlb_entry) * hartptr->itlb_cap);
    hartptr->dtlb = aligned_alloc(64, sizeof(struct tlb_entry) * hartptr->dtlb_cap);
    ASSERT(hartptr->itlb && hartptr->dtlb);
    invalidate_tlb(hartptr->itlb, hartptr->itlb_cap);
    invalidate_tlb(hartptr->dtlb, hartptr->dtlb_cap);
}

static void
cpu_init(struct virtual_machine * vm)
{
    const char * nr_cpus_string = ini_get(vm->ini_config, "cpu", "nr_cpus");
    const char * boot_cpu_string = ini_get(vm->ini_config, "cpu", "boot_cpu");
    ASSERT(nr_cpus_string);
    vm->nr_harts = strtol(nr_cpus_string, NULL, 10);
    vm->boot_hart = 0;
    if (boot_cpu_string) {
        vm->boot_hart = strtol(boot_cpu_string, NULL, 10);   
    }
    vm->harts = aligned_alloc(64, vm->nr_harts * sizeof(struct hart));
    ASSERT(vm->harts);

    const char * pc_on_reset_string = ini_get(vm->ini_config, "cpu", "pc_on_reset");
    ASSERT(pc_on_reset_string);
    uint32_t pc_on_reset = strtol(pc_on_reset_string, NULL, 16);

    int idx = 0;
    for (; idx < vm->nr_harts; idx++) {
        struct hart * hartptr = hart_by_id(vm, idx);
        hart_init(hartptr, idx);
        hartptr->pc = pc_on_reset;
        hartptr->vmptr = vm;
    }

    const char * itlb_size_string = ini_get(vm->ini_config, "cpu", "itlb_size");
    const char * dtlb_size_string = ini_get(vm->ini_config, "cpu", "dtlb_size");
    uint32_t itlb_size = strtol(itlb_size_string, NULL, 16);
    uint32_t dtlb_size = strtol(dtlb_size_string, NULL, 16);
    log_debug("itlb size:%d dtlb size:%d\n", itlb_size, dtlb_size);
    for (idx = 0; idx < vm->nr_harts; idx++) {
        struct hart * hartptr = hart_by_id(vm, idx);
        hartptr->itlb_cap = itlb_size;
        hartptr->dtlb_cap = dtlb_size;
        hart_tlb_init(hartptr);
    }
}

static void
misc_init(struct virtual_machine * vm)
{
    // Load breakpoints if there is any.
    char * breakpoints = (char *)ini_get(vm->ini_config, "debug", "breakpoints");
    char delimiter[] = " ";
    if (breakpoints) {
        char * bp = strtok(breakpoints, delimiter);
        while (bp) {
            add_breakpoint(strtol(bp, NULL, 16));
            bp = strtok(NULL, delimiter);
        }
    }
}

void
virtual_machine_init(struct virtual_machine * vm, ini_t * ini)
{
    memset(vm, 0x0, sizeof(struct virtual_machine));
    // nothing can precede ini configuration
    vm->ini_config = ini;

    bootrom_init(vm);
    ram_init(vm);
    cpu_init(vm);
    device_init(vm);
    misc_init(vm);
    fdt_init(&vm->fdt);

    dump_memory_regions();
}

