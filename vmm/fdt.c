/*
 * Copyright (c) 2020 Jie Zheng
 */

#include <fdt.h>
#define FDT_TEST 1
#include <string.h>

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
                ptr = ALIGN4(ptr);
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
                    ptr = ALIGN4(ptr);
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
    log_debug("fdt.totalsize:%d\n", LITTLE_ENDIAN32(fdt->totalsize));
    log_debug("fdt.version:%d\n", LITTLE_ENDIAN32(fdt->version));
    log_debug("fdt.last_comp_version:%d\n", LITTLE_ENDIAN32(fdt->last_comp_version));
    log_debug("fdt.off_dt_strings:%d\n", LITTLE_ENDIAN32(fdt->off_dt_strings));
    scan_fdt_tokens(dtb, size);
}



#if FDT_TEST
    #include <stdio.h>
    #include<fcntl.h>
    #include <sys/stat.h>
    #include <unistd.h>
#endif

__attribute__((constructor)) static void
fdt_test(void)
{
#if FDT_TEST
    uint8_t * dtb_base = malloc(8192);
    ASSERT(dtb_base);
    log_info("dtb_base: %p\n", dtb_base);
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
#endif
}


