/*
 * Copyright (c) 2019-2020 Jie Zheng
 */
#include <stdint.h>
#include <uart.h>
#include <printk.h>

int
init_main(uint32_t boot_hartid, uint8_t * dtb)
{
    int a = 4;
    int b = 12;
    uart16550_init();

    printk("boot hartid:%d\n", boot_hartid);
    printk("dtb base: 0x%x\n", dtb);
    printk("Hello World from %s\n", "guest");
    printk("hex:%x dec:%d char:%c\n", 0xffee000f, -2345, 'c');
    while (0) {
        __attribute__((unused)) int c = a + b;
    }
    return 0;
}
