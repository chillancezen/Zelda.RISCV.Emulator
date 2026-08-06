/*
 * Copyright (c) 2020 Jie Zheng
 *
 *      The handoff block the vmm leaves in rom for the firmware.
 *
 *      The firmware runs before any device tree parsing is possible, so the
 *      addresses it needs to boot - where the kernel was loaded, where the
 *      device tree ended up - are deposited at a fixed rom address instead.
 *      This header is shared verbatim by the vmm and the firmware; keep the
 *      two in step.
 */

#ifndef _ZELDA_BOOT_INFO_H
#define _ZELDA_BOOT_INFO_H

/*
 * The rom region is laid out as:
 *   0x0000 - 0x1000  unused
 *   0x1000 - 0x3000  device tree blob
 *   0x3000 - 0x4000  this handoff block
 *   0x4000 -         firmware image, which is also the reset vector
 *
 * The region has to start at physical zero and be at least four megabytes
 * long. A supervisor reaching the device tree maps the whole four megabyte
 * page that contains it, so every byte of that page must be backed by
 * something, and the page containing 0x1000 starts at zero.
 */
#define ZELDA_ROM_BASE          0x0000
#define ZELDA_ROM_MIN_SIZE      0x400000
#define ZELDA_DTB_ADDRESS       0x1000
#define ZELDA_DTB_MAX_SIZE      0x2000
#define ZELDA_BOOT_INFO_ADDRESS 0x3000
#define ZELDA_BOOT_INFO_MAGIC   0x5a424f49  /* "ZBOI" */

#ifndef __ASSEMBLER__
#include <stdint.h>

struct zelda_boot_info {
    uint32_t magic;
    uint32_t kernel_entry;
    uint32_t dtb_address;
    uint32_t initrd_start;
    uint32_t initrd_end;
    uint32_t memory_start;
    uint32_t memory_size;
    uint32_t nr_harts;
};
#endif

#endif
