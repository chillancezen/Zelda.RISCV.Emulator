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


struct fdt_build_blob {
    uint8_t * buffer;
    uint32_t buffer_size;
    uint32_t buffer_iptr;

    uint8_t * string_buffer;
    uint32_t string_buffer_size;
    uint32_t string_buffer_iptr;
};


void
fdt_build_init(struct fdt_build_blob * blob, int buffer_size,
               int string_buffer_size);


void
fdt_begin_node(struct fdt_build_blob * blob, const char * node_name);

void
fdt_prop(struct fdt_build_blob * blob, const char * prop_name,
         uint32_t prop_len, void * prop_value);

void
fdt_end_node(struct fdt_build_blob * blob);


void
fdt_nop(struct fdt_build_blob * blob);


void
fdt_end(struct fdt_build_blob * blob);


void
fdt_blob_destroy(struct fdt_build_blob * blob);


void
build_device_tree(struct fdt_build_blob * blob);

void
dump_fdt(uint8_t * dtb, int size);


// DEVICES FDT BUILD ROUTINES
void
build_uart_fdt_node(struct fdt_build_blob * blob);

#endif
