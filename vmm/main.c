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


static uint32_t bps[] = {
    //0x1001d8, 0x100238, 0x100204
};
int main(int argc, char ** argv)
{
    struct virtual_machine_spec spec = {
        .nr_harts = 2,
        .pmem_size_in_mega = 64,
        .image_path = "../guest/guest.rv32.img",
        .image_load_offset = 0x100000,
        .entry_point = 0x100000
    };

    struct virtual_machine vm0;
    virtual_machine_init(&vm0, &spec);
    int idx = 0;
    for (; idx < sizeof(bps)/sizeof(bps[0]); idx++) {
        add_breakpoint(bps[idx]);
    }
    vmresume(hart_by_id(&vm0, 1));
    __asm__ volatile(".byte 0xcc;");
    return 0;
}
