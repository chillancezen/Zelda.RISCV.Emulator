/*
 * Copyright (c) 2020 Jie Zheng
 */

#ifndef _FDT_H
#define _FDT_H
#include <stdint.h>

#define FDT_MAGIC 0xd00dfeed

struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
}__attribute__((packed));

#define LITTLE_ENDIAN32(v) ((((v) & 0xff) << 24) |                             \
                            (((v) & 0xff00) << 8) |                            \
                            (((v) & 0xff0000) >> 8) |                          \
                            (((v) & 0xff000000) >> 24))

#define BIG_ENDIAN32(v) LITTLE_ENDIAN32(v)

void
validate_dtb(void);
#endif
