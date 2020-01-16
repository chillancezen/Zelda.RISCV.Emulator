/*
 * Copyright (c) 2020 Jie Zheng
 */
#include <fdt.h>
#include <sha1.h>
#include <printk.h>

void
validate_dtb(void)
{
    uint8_t * dtb_base = (uint8_t *)0x1000;
    struct fdt_header * fdt = (struct fdt_header *)dtb_base;
    if (fdt->magic == BIG_ENDIAN32(FDT_MAGIC)) {
        printk("device tree detected at 0x%x\n", 0x1000);
        printk("device tree blob size: %d bytes\n", LITTLE_ENDIAN32(fdt->totalsize));
        int dtb_size = LITTLE_ENDIAN32(fdt->totalsize);
        char * index_tbl="0123456789abcdef";
        int idx = 0;
        char result[21];
        char hex_result[41];
        SHA1(result, (const char *)dtb_base, dtb_size);
        for (idx = 0; idx < 20; idx++) {
            hex_result[idx * 2 + 1] = index_tbl[result[idx] & 0xf];
            hex_result[idx * 2] = index_tbl[(result[idx] >> 4) & 0xf];
        }
        hex_result[40] = '\0';
        printk("device tree blob sha1 checksum: %s\n", hex_result);
    } else {
        printk(ANSI_COLOR_RED"device tree not detected\n"ANSI_COLOR_RESET);
    }
}
