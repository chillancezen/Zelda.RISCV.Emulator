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

static void
device_init(struct virtual_machine * vm)
{
    uart_init();
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

