/*
 * Copyright (c) 2019 Jie Zheng
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


static int
get_vm_spec(struct virtual_machine_spec * spec, ini_t * ini_config)
{
    memset(spec, 0x0, sizeof(struct virtual_machine_spec));
    // number of harts
    const char * nr_harts = ini_get(ini_config, "cpu", "nr_cpus");
    ASSERT(nr_harts);
    spec->nr_harts = atoi(nr_harts);
    log_info("number of cpus: %d\n", spec->nr_harts);
    // boot cpu index
    const char * boot_cpu = ini_get(ini_config, "cpu", "boot_cpu");
    if (boot_cpu) {
        spec->boot_cpu = atoi(boot_cpu);
    }
    log_info("boot cpu index: %d\n", spec->boot_cpu);

    // image type
    const char * image_type = ini_get(ini_config, "guest", "type");
    ASSERT(image_type);
    if (!strcmp(image_type, "binary")) {
        spec->image_type = IMAGE_TYPE_BINARY;
    } else if(!strcmp(image_type, "elf")) {
        spec->image_type = IMAGE_TYPE_ELF;
    } else {
        __not_reach();
    }
    log_info("image type: %s\n", spec->image_type == IMAGE_TYPE_BINARY ? "binary" : "elf");

    // image path
    const char * image_path = ini_get(ini_config, "guest", "path");
    ASSERT(image_path);
    spec->image_path = (char *)image_path;
    log_info("image path: %s\n", spec->image_path);

    // load_base
    if (spec->image_type == IMAGE_TYPE_BINARY) {
        const char * load_base = ini_get(ini_config, "guest", "load_base");
        ASSERT(load_base);
        spec->image_load_base = strtol(load_base, NULL, 16);
        log_info("image load at: 0x%x\n", spec->image_load_base);
    }
    // entry_point
    if (spec->image_type == IMAGE_TYPE_BINARY) {
        const char * entry_point = ini_get(ini_config, "guest", "entry_point");
        ASSERT(entry_point)
        spec->entry_point = strtol(entry_point, NULL, 16);
        log_info("image entry point: 0x%x\n", spec->entry_point);
    }

    // main-memory base
    const char * main_memory_start = ini_get(ini_config, "mem",
                                             "main_memory_start");
    ASSERT(main_memory_start);
    spec->main_mem_base = strtol(main_memory_start, NULL, 16);
    log_info("main memory base: 0x%x\n", spec->main_mem_base);

    // main-memory size
    const char * main_memory_size_in_mega = ini_get(ini_config, "mem",
                                                    "main_memory_size_in_mega");
    ASSERT(main_memory_size_in_mega);
    spec->main_mem_size_in_mega = strtol(main_memory_size_in_mega, NULL, 10);
    log_info("main memory size in MB: %d\n", spec->main_mem_size_in_mega);

    // initial breakpoints
    // XXX: this is going to damage the ini_config, put it at last section.
    char * breakpoints = (char *)ini_get(ini_config, "debug", "breakpoints");
    char delimiter[] = " ";
    if (breakpoints) {
        char * bp = strtok(breakpoints, delimiter);
        while (bp) {
            add_breakpoint(strtol(bp, NULL, 16));
            bp = strtok(NULL, delimiter);
        }
    }
    return 0;
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

    struct virtual_machine_spec spec;
    get_vm_spec(&spec, ini_config);

    struct virtual_machine vm0;
    virtual_machine_init(&vm0, &spec);

    vmresume(hart_by_id(&vm0, vm0.boot_hart));
    __asm__ volatile(".byte 0xcc;");
    return 0;
}
