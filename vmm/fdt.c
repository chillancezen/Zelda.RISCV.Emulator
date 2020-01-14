/*
 * Copyright (c) 2020 Jie Zheng
 */

#include <fdt.h>
#include <string.h>
#include <malloc.h> 
#include<fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

void
fdt_build_init(struct fdt_build_blob * blob, int buffer_size,
               int string_buffer_size)
{
    memset(blob, 0, sizeof(struct fdt_build_blob));
    blob->buffer_size = buffer_size;
    blob->buffer = malloc(buffer_size);
    ASSERT(blob->buffer);
    memset(blob->buffer, 0, buffer_size);

    blob->string_buffer_size = string_buffer_size;
    blob->string_buffer = malloc(string_buffer_size);
    ASSERT(blob->string_buffer);
    memset(blob->string_buffer, 0, string_buffer_size);

    struct fdt_header * fdt = (struct fdt_header *)blob->buffer;
    fdt->magic = BIG_ENDIAN32(FDT_MAGIC);
    fdt->version = BIG_ENDIAN32(17);
    fdt->last_comp_version = BIG_ENDIAN32(2);
    blob->buffer_iptr = sizeof(struct fdt_header);
    blob->buffer_iptr = ALIGN32(blob->buffer_iptr, 4);

    // insert an empty memory reservation block right after the fdt header
    fdt->off_mem_rsvmap = BIG_ENDIAN32(blob->buffer_iptr);
    blob->buffer_iptr += 16;
    blob->buffer_iptr = ALIGN32(blob->buffer_iptr, 4);

    // begin the structures
    fdt->off_dt_struct = BIG_ENDIAN32(blob->buffer_iptr);
}


void
fdt_begin_node(struct fdt_build_blob * blob, const char * node_name)
{
    uint32_t room_available = blob->buffer_size - blob->buffer_iptr;
    uint32_t room_needed = 4 + strlen(node_name) + 1;
    room_needed = ALIGN32(room_needed, 4);
    ASSERT(room_needed <= room_available);
    *(uint32_t *)(blob->buffer + blob->buffer_iptr) = BIG_ENDIAN32(FDT_BEGIN_NODE);
    memcpy(blob->buffer + blob->buffer_iptr + 4, node_name, strlen(node_name));
    blob->buffer_iptr += room_needed;
}

void
fdt_prop(struct fdt_build_blob * blob, const char * prop_name,
         uint32_t prop_len, void * prop_value)
{
    uint32_t room_available = blob->buffer_size - blob->buffer_iptr;
    uint32_t room_needed = 4 + sizeof(struct fdt_property) + prop_len;
    room_needed = ALIGN32(room_needed, 4);
    ASSERT(room_needed <= room_available);
    *(uint32_t *)(blob->buffer + blob->buffer_iptr) = BIG_ENDIAN32(FDT_PROP);
    struct fdt_property * fdt_prop =
        (struct fdt_property *)(blob->buffer + blob->buffer_iptr + 4);
    fdt_prop->len = BIG_ENDIAN32(prop_len);
    fdt_prop->nameoff = BIG_ENDIAN32(blob->string_buffer_iptr);
    memcpy(blob->buffer + blob->buffer_iptr + 4 + sizeof(struct fdt_property),
           prop_value, prop_len);
    blob->buffer_iptr += room_needed;
    // make room for the property name string
    uint32_t name_size = strlen(prop_name) + 1;
    ASSERT(name_size < (blob->string_buffer_size - blob->string_buffer_iptr));
    memcpy(blob->string_buffer + blob->string_buffer_iptr,
           prop_name, name_size);
    blob->string_buffer_iptr += name_size;
}

void
fdt_end_node(struct fdt_build_blob * blob)
{
    uint32_t room_available = blob->buffer_size - blob->buffer_iptr;
    uint32_t room_needed = 4;
    ASSERT(room_needed <= room_available);
    *(uint32_t *)(blob->buffer + blob->buffer_iptr) = BIG_ENDIAN32(FDT_END_NODE);
    blob->buffer_iptr += room_needed;
}

void
fdt_nop(struct fdt_build_blob * blob)
{
    uint32_t room_available = blob->buffer_size - blob->buffer_iptr;
    uint32_t room_needed = 4;
    ASSERT(room_needed <= room_available);
    *(uint32_t *)(blob->buffer + blob->buffer_iptr) = BIG_ENDIAN32(FDT_NOP);
    blob->buffer_iptr += room_needed;
}


void
fdt_end(struct fdt_build_blob * blob)
{
    uint32_t room_available = blob->buffer_size - blob->buffer_iptr;
    uint32_t room_needed = 4;
    ASSERT(room_needed <= room_available);
    *(uint32_t *)(blob->buffer + blob->buffer_iptr) = BIG_ENDIAN32(FDT_END);
    blob->buffer_iptr += room_needed;
    // there should be NO any tokens.
    // merge two buffers and fill other fields
    struct fdt_header * fdt = (struct fdt_header *)blob->buffer;
    fdt->off_dt_strings = BIG_ENDIAN32(blob->buffer_iptr);
    fdt->size_dt_strings = BIG_ENDIAN32(blob->string_buffer_iptr);
    uint32_t size_struct = blob->buffer_iptr - LITTLE_ENDIAN32(fdt->off_dt_struct);
    fdt->size_dt_struct = BIG_ENDIAN32(size_struct);
    uint32_t size_total = blob->buffer_iptr + blob->string_buffer_iptr;
    fdt->totalsize = BIG_ENDIAN32(size_total);
    // copy string buffer
    room_available = blob->buffer_size - blob->buffer_iptr;
    room_needed = blob->string_buffer_iptr;
    ASSERT(room_needed <= room_available);
    memcpy(blob->buffer + blob->buffer_iptr, blob->string_buffer,
           blob->string_buffer_iptr);
    blob->buffer_iptr += blob->string_buffer_iptr;
}

void
fdt_blob_destroy(struct fdt_build_blob * blob)
{
    if (blob->buffer) {
        free(blob->buffer);
        blob->buffer = NULL;
    }

    if (blob->string_buffer) {
        free(blob->string_buffer);
        blob->string_buffer = NULL;
    }
    memset(blob, 0, sizeof(struct fdt_build_blob));
}

void
dump_device_tree_to_file(struct fdt_build_blob * blob,
                         char * dtb_path)
{
    int fd = open(dtb_path, O_CREAT | O_WRONLY);

    int nr_left = blob->buffer_iptr;
    int iptr = 0;

    while (nr_left > 0) {
        int rc = write(fd, blob->buffer + iptr, nr_left);
        if (rc <= 0) {
            break;
        }

        nr_left -= rc;
        iptr += rc;
    }
    close(fd);
}


void
build_device_tree(struct fdt_build_blob * blob)
{
    fdt_build_init(blob, 4096, 512);
    fdt_begin_node(blob, ""); // the root node
    uint32_t address_cells = BIG_ENDIAN32(2);
    uint32_t size_cells = BIG_ENDIAN32(2);
    fdt_prop(blob, "#address-cells", 4, &address_cells);
    fdt_prop(blob, "#size-cells", 4, &size_cells);
    fdt_prop(blob, "compatible", strlen("riscv-virtio") + 1, "riscv-virtio");
    fdt_prop(blob, "model", strlen("riscv-virtio,qemu") + 1, "riscv-virtio,qemu");

    build_uart_fdt_node(blob);
    fdt_end_node(blob);
    fdt_end(blob);
    dump_device_tree_to_file(blob, "/root/workspace/diskimage/zelda.dtb");
}

void
scan_fdt_tokens(uint8_t * dtb, int size)
{
    struct fdt_header * fdt = (struct fdt_header *)dtb;
    uint8_t * fdt_struct_base = dtb + LITTLE_ENDIAN32(fdt->off_dt_struct);
    uint8_t * ptr = fdt_struct_base;
    char * node_name = NULL;
    char * node_prop_name = NULL;
    int node_prop_length = NULL;
    void * node_prop_value = NULL;
    int to_stop = 0;
    while(!to_stop) {
        uint32_t token = LITTLE_ENDIAN32(*(uint32_t *)ptr);
        ptr += 4;
        switch(token)
        {
            case FDT_BEGIN_NODE:
                node_name = (char *)ptr;
                ptr += strlen(node_name) + 1;
                ptr = ALIGNADDR(ptr, 4);
                break;
            case FDT_PROP:
                {
                    struct fdt_property *prop = (struct fdt_property *)ptr;
                    node_prop_length = LITTLE_ENDIAN32(prop->len);
                    node_prop_name = (char *)(dtb + LITTLE_ENDIAN32(fdt->off_dt_strings) +
                                              LITTLE_ENDIAN32(prop->nameoff));
                    ptr += sizeof(struct fdt_property);
                    node_prop_value = ptr;
                    ptr += node_prop_length;
                    ptr = ALIGNADDR(ptr, 4);
                    log_debug("%s.%s length:%d <@%p>\n", node_name,
                              node_prop_name, node_prop_length,
                              node_prop_value);
                }
                break;
            case FDT_NOP:
                break;
            case FDT_END_NODE:
                break;
            case FDT_END:
                to_stop = 1;
                break;
            default:
                __not_reach();
                break;
        }
    }
}

void
dump_fdt(uint8_t * dtb, int size)
{
    struct fdt_header * fdt = (struct fdt_header *)dtb;
    log_debug("fdt.magicword:0x%x\n", LITTLE_ENDIAN32(fdt->magic));
    log_debug("fdt.version:%d\n", LITTLE_ENDIAN32(fdt->version));
    log_debug("fdt.last_comp_version:%d\n", LITTLE_ENDIAN32(fdt->last_comp_version));
    log_debug("fdt.off_dt_strings:%d\n", LITTLE_ENDIAN32(fdt->off_dt_strings));
    log_debug("fdt.off_dt_struct:%d\n", LITTLE_ENDIAN32(fdt->off_dt_struct));
    log_debug("fdt.off_mem_rsvmap:%d\n", LITTLE_ENDIAN32(fdt->off_mem_rsvmap));
    log_debug("fdt.size_dt_strings:%d\n", LITTLE_ENDIAN32(fdt->size_dt_strings));
    log_debug("fdt.size_dt_struct:%d\n", LITTLE_ENDIAN32(fdt->size_dt_struct));
    log_debug("fdt.totalsize:%d\n", LITTLE_ENDIAN32(fdt->totalsize));
    scan_fdt_tokens(dtb, size);
}

#define FDT_TEST 0
#if FDT_TEST
    #include <stdio.h>
    #include<fcntl.h>
    #include <sys/stat.h>
    #include <unistd.h>

__attribute__((constructor)) static void
fdt_test(void)
{
    struct fdt_build_blob blob;
    fdt_build_init(&blob, 8192, 512);
    fdt_begin_node(&blob, "");
    
    uint32_t address_cells = BIG_ENDIAN32(2);
    uint32_t size_cells = BIG_ENDIAN32(2);
    fdt_prop(&blob, "#address-cells", 4, &address_cells);
    fdt_prop(&blob, "#size-cells", 4, &size_cells);
    fdt_prop(&blob, "compatible", strlen("riscv-virtio") + 1, "riscv-virtio");
    fdt_prop(&blob, "model", strlen("riscv-virtio,qemu") + 1, "riscv-virtio,qemu");
    fdt_begin_node(&blob, "uart@10000000");
    fdt_prop(&blob, "compatible", strlen("ns16550a") + 1, "ns16550a");
    fdt_end_node(&blob);
    fdt_end_node(&blob);
    fdt_end(&blob);
    dump_fdt(blob.buffer, blob.buffer_iptr);
    int fd0 = open("../diskimage/zelda.dtb", O_CREAT | O_WRONLY);
    write(fd0, blob.buffer, blob.buffer_iptr);
    return;

    uint8_t * dtb_base = malloc(8192);
    ASSERT(dtb_base);
    log_debug("dtb_base: %p\n", dtb_base);
    int fd = open("../diskimage/riscv32-virt.dtb", O_RDONLY);
    int nr_left = 8192;
    int dtb_length = 0;
    
    while(nr_left > 0) {
        int rc = read(fd, dtb_base + dtb_length, nr_left);
        if (rc <= 0) {
            break;
        }
        nr_left -= rc;
        dtb_length += rc;   
    }
    ASSERT(dtb_length < nr_left);
    dump_fdt(dtb_base, dtb_length);
    close(fd);
}
#endif
