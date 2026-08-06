/*
 * Copyright (c) 2019-2020 Jie Zheng
 *
 *      Machine mode firmware: brings the platform up, then drops into the
 *      supervisor kernel and stays behind to service its SBI calls.
 */
#include <stdint.h>
#include <printk.h>
#include <unittest.h>
#include <uart.h>
#include <fdt.h>
#include <sbi.h>
#include <csr.h>
#include <platform.h>
#include <zelda_boot_info.h>

/*
 * Everything the supervisor is expected to cope with on its own. Leaving an
 * exception out of this set means every occurrence takes a round trip through
 * machine mode, and page faults in particular would make the kernel unusably
 * slow - or, since we do not forward them, dead.
 */
#define MEDELEG_TO_SUPERVISOR                                                  \
    ((1u << CAUSE_MISALIGNED_FETCH)     |                                      \
     (1u << CAUSE_FETCH_ACCESS)         |                                      \
     (1u << CAUSE_ILLEGAL_INSTRUCTION)  |                                      \
     (1u << CAUSE_BREAKPOINT)           |                                      \
     (1u << CAUSE_MISALIGNED_LOAD)      |                                      \
     (1u << CAUSE_LOAD_ACCESS)          |                                      \
     (1u << CAUSE_MISALIGNED_STORE)     |                                      \
     (1u << CAUSE_STORE_ACCESS)         |                                      \
     (1u << CAUSE_USER_ECALL)           |                                      \
     (1u << CAUSE_FETCH_PAGE_FAULT)     |                                      \
     (1u << CAUSE_LOAD_PAGE_FAULT)      |                                      \
     (1u << CAUSE_STORE_PAGE_FAULT))

// The supervisor's own three interrupts belong to it directly.
#define MIDELEG_TO_SUPERVISOR (MIP_SSIP | MIP_STIP | MIP_SEIP)

static const struct zelda_boot_info *
boot_info(void)
{
    return (const struct zelda_boot_info *)ZELDA_BOOT_INFO_ADDRESS;
}

static void
banner(const struct zelda_boot_info * info)
{
    printk("\r\n");
    printk("zelda machine mode firmware\r\n");
    printk("  platform    : rv32ima, %d hart(s)\r\n", info->nr_harts);
    printk("  memory      : 0x%x + %d MiB\r\n", info->memory_start,
           info->memory_size >> 20);
    printk("  device tree : 0x%x\r\n", info->dtb_address);
    printk("  kernel      : 0x%x\r\n", info->kernel_entry);
    if (info->initrd_end > info->initrd_start) {
        printk("  initrd      : 0x%x - 0x%x\r\n", info->initrd_start,
               info->initrd_end);
    }
    printk("  sbi         : v0.3, extensions base/time/ipi/rfence/srst\r\n");
    printk("\r\n");
}

void
fw_main(void)
{
    const struct zelda_boot_info * info = boot_info();
    uint32_t mstatus;

    uart16550_init();

    if (info->magic != ZELDA_BOOT_INFO_MAGIC) {
        printk("[firmware] no boot info at 0x%x, cannot continue\r\n",
               ZELDA_BOOT_INFO_ADDRESS);
        for (;;) {
            __asm__ volatile("wfi");
        }
    }

    banner(info);
    /*
     * The instruction level self tests are a check on the emulator rather than
     * on the guest, and they are noisy and slow. Build with FW_SELFTEST=1 to
     * run them.
     */
#if defined(FW_SELFTEST)
    unit_test();
#endif
    validate_dtb(info->dtb_address);

    sbi_init();

    write_csr(medeleg, MEDELEG_TO_SUPERVISOR);
    write_csr(mideleg, MIDELEG_TO_SUPERVISOR);

    /*
     * Return to supervisor mode with its interrupts enabled: MPP selects the
     * privilege mret drops to, and MPIE becomes the supervisor's SIE.
     */
    mstatus = read_csr(mstatus);
    mstatus &= ~MSTATUS_MPP_MASK;
    mstatus |= PRIVILEGE_SUPERVISOR << MSTATUS_MPP_SHIFT;
    mstatus |= MSTATUS_MPIE;
    write_csr(mstatus, mstatus);

    printk("[firmware] entering supervisor at 0x%x\r\n\r\n",
           info->kernel_entry);

    enter_supervisor(read_csr(mhartid), info->dtb_address, info->kernel_entry);
}
