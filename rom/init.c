/*
 * Copyright (c) 2019-2020 Jie Zheng
 */
#include <stdint.h>
#include <printk.h>
#include <unittest.h>
#include <sha1.h>
#include <uart.h>

void
rom_init(void)
{
    uart16550_init();
    // perform basic unit tests
    unit_test();

    // at last we jump to guest image, and never be back
    uint32_t hart_id = 0x0;
    void * dtb = (void *)0x1000;
    ((void (*)(uint32_t, void *))0x80000000)(hart_id, dtb);
}

