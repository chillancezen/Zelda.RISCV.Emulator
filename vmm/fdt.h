/*
 * Copyright (c) 2020 Jie Zheng
 */
#ifndef _FDT_H
#define _FDT_H

#include <util.h>
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

// the tokens of structure
#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

struct fdt_property {
    uint32_t len;
    uint32_t nameoff;
}__attribute__((packed));

#endif
