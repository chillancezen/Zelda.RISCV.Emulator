/*
 * Copyright (c) 2019-2020 Jie Zheng
 */

#include <stdio.h>
#include <physical_memory.h>
#include <util.h>
#include <hart.h>
#include <vm.h>
#include <mmu.h>
#include <translation.h>
#include <debug.h>
#include <ini.h>


static void
vmm_misc_init(ini_t * ini)
{
    const char * verbosity_string = ini_get(ini, "debug", "verbosity");
    if (verbosity_string) {
        int verbosity = strtol(verbosity_string, NULL, 10);
        log_set_level(verbosity);
    }
}
int main(int argc, char ** argv)
{
    if (argc <= 1) {
        log_fatal("please specify the vm config file\n");
        exit(1);
    }
    // once config file is loaded, don't release it ever.
    ini_t * ini_config = ini_load(argv[1]);
    if (!ini_config) {
        log_fatal("can not load ini file:%s\n", argv[1]);
        exit(2);
    }
    // vmm platform init
    vmm_misc_init(ini_config);
    // Boot a VM
    struct virtual_machine vm;
    virtual_machine_init(&vm, ini_config);
    vmresume(hart_by_id(&vm, vm.boot_hart));
    __not_reach();
    return 0;
}
