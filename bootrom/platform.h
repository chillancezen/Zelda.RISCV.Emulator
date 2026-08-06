/*
 * Copyright (c) 2020 Jie Zheng
 *
 *      Fixed addresses of the emulated platform. These must agree with the
 *      regions the vmm registers and with the device tree it generates.
 */

#ifndef _PLATFORM_H
#define _PLATFORM_H

#define UART16550_BASE          0x10000000

#define CLINT_BASE              0x02000000
#define CLINT_MSIP_OFFSET       0x0000
#define CLINT_MTIMECMP_OFFSET   0x4000
#define CLINT_MTIME_OFFSET      0xbff8

#define PLIC_BASE               0x0c000000

/*
 * Test finisher, matching the sifive,test0 device. Writing the poweroff code
 * asks the vmm to stop the machine, which is how "poweroff" in the guest gets
 * the emulator to exit.
 */
#define SYSCON_BASE             0x00400000
#define SYSCON_POWEROFF         0x00005555
#define SYSCON_REBOOT           0x00007777

#endif
