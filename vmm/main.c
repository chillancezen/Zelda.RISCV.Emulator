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


    struct virtual_machine vm0;
    virtual_machine_init(&vm0, ini_config);
    vmresume(hart_by_id(&vm0, vm0.boot_hart));
    __asm__ volatile(".byte 0xcc;");
    return 0;
}
