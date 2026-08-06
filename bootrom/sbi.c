/*
 * Copyright (c) 2020 Jie Zheng
 *
 *      Machine mode trap handling and the SBI the kernel calls into.
 */

#include <sbi.h>
#include <csr.h>
#include <uart.h>
#include <printk.h>
#include <platform.h>

static inline uint32_t
current_hartid(void)
{
    return read_csr(mhartid);
}

/*
 * Arm the machine timer on behalf of the supervisor.
 *
 * The supervisor timer interrupt has no hardware of its own: only machine
 * mode can reach the CLINT. So the firmware owns mtimecmp, and turns the
 * machine timer interrupt it receives into the supervisor timer interrupt the
 * kernel is waiting for.
 */
static void
sbi_set_timer(uint64_t next)
{
    volatile uint32_t * mtimecmp =
        (volatile uint32_t *)(CLINT_BASE + CLINT_MTIMECMP_OFFSET +
                              8 * current_hartid());

    /*
     * Park the comparator at its maximum while the halves are inconsistent,
     * so a value that is briefly in the past cannot fire a spurious
     * interrupt.
     */
    mtimecmp[1] = 0xffffffff;
    mtimecmp[0] = (uint32_t)next;
    mtimecmp[1] = (uint32_t)(next >> 32);

    // The pending supervisor timer interrupt, if any, has been consumed.
    clear_csr(mip, MIP_STIP);
    set_csr(mie, MIE_MTIE);
}

static void
sbi_send_ipi(uint32_t hart)
{
    volatile uint32_t * msip =
        (volatile uint32_t *)(CLINT_BASE + CLINT_MSIP_OFFSET + 4 * hart);
    *msip = 1;
}

static void
sbi_shutdown(void)
{
    volatile uint32_t * finisher = (volatile uint32_t *)SYSCON_BASE;
    printk("\r\nzelda: system halted\r\n");
    *finisher = SYSCON_POWEROFF;
    // If the platform did not take us up on it, stop the hart.
    for (;;) {
        __asm__ volatile("wfi");
    }
}

static long
sbi_probe_extension(uint32_t extension_id)
{
    switch (extension_id)
    {
        case SBI_EXT_BASE:
        case SBI_EXT_TIME:
        case SBI_EXT_IPI:
        case SBI_EXT_RFENCE:
        case SBI_EXT_SRST:
        case SBI_EXT_LEGACY_SET_TIMER:
        case SBI_EXT_LEGACY_CONSOLE_PUTCHAR:
        case SBI_EXT_LEGACY_CONSOLE_GETCHAR:
        case SBI_EXT_LEGACY_CLEAR_IPI:
        case SBI_EXT_LEGACY_SEND_IPI:
        case SBI_EXT_LEGACY_REMOTE_FENCE_I:
        case SBI_EXT_LEGACY_REMOTE_SFENCE_VMA:
        case SBI_EXT_LEGACY_REMOTE_SFENCE_VMA_ASID:
        case SBI_EXT_LEGACY_SHUTDOWN:
            return 1;
        default:
            return 0;
    }
}

/*
 * The pre-0.2 calls return a single value in a0 and have no error register.
 */
static void
sbi_legacy_call(struct trap_frame * frame, uint32_t extension_id)
{
    long ret = 0;

    switch (extension_id)
    {
        case SBI_EXT_LEGACY_SET_TIMER:
            // rv32 splits the 64 bit deadline across a0 and a1.
            sbi_set_timer(((uint64_t)frame->regs[REG_A1] << 32) |
                          frame->regs[REG_A0]);
            break;
        case SBI_EXT_LEGACY_CONSOLE_PUTCHAR:
            uart16550_putchar((uint8_t)frame->regs[REG_A0]);
            break;
        case SBI_EXT_LEGACY_CONSOLE_GETCHAR:
            ret = uart16550_getchar();
            break;
        case SBI_EXT_LEGACY_CLEAR_IPI:
            clear_csr(mip, MIP_SSIP);
            break;
        case SBI_EXT_LEGACY_SEND_IPI:
            // Uniprocessor: the only possible target is ourselves.
            sbi_send_ipi(current_hartid());
            break;
        case SBI_EXT_LEGACY_REMOTE_FENCE_I:
            __asm__ volatile("fence.i" ::: "memory");
            break;
        case SBI_EXT_LEGACY_REMOTE_SFENCE_VMA:
        case SBI_EXT_LEGACY_REMOTE_SFENCE_VMA_ASID:
            __asm__ volatile("sfence.vma" ::: "memory");
            break;
        case SBI_EXT_LEGACY_SHUTDOWN:
            sbi_shutdown();
            break;
        default:
            ret = SBI_ERR_NOT_SUPPORTED;
            break;
    }
    frame->regs[REG_A0] = (uint32_t)ret;
}

/*
 * From 0.2 onwards every call returns an error code in a0 and a value in a1.
 */
static void
sbi_modern_call(struct trap_frame * frame, uint32_t extension_id,
                uint32_t function_id)
{
    long error = SBI_SUCCESS;
    long value = 0;

    switch (extension_id)
    {
        case SBI_EXT_BASE:
            switch (function_id)
            {
                case SBI_BASE_GET_SPEC_VERSION:
                    value = SBI_SPEC_VERSION;
                    break;
                case SBI_BASE_GET_IMPL_ID:
                    value = SBI_IMPL_ID;
                    break;
                case SBI_BASE_GET_IMPL_VERSION:
                    value = SBI_IMPL_VERSION;
                    break;
                case SBI_BASE_PROBE_EXTENSION:
                    value = sbi_probe_extension(frame->regs[REG_A0]);
                    break;
                case SBI_BASE_GET_MVENDORID:
                    value = read_csr(mvendorid);
                    break;
                case SBI_BASE_GET_MARCHID:
                    value = read_csr(marchid);
                    break;
                case SBI_BASE_GET_MIMPID:
                    value = read_csr(mimpid);
                    break;
                default:
                    error = SBI_ERR_NOT_SUPPORTED;
                    break;
            }
            break;

        case SBI_EXT_TIME:
            if (function_id == 0) {
                sbi_set_timer(((uint64_t)frame->regs[REG_A1] << 32) |
                              frame->regs[REG_A0]);
            } else {
                error = SBI_ERR_NOT_SUPPORTED;
            }
            break;

        case SBI_EXT_IPI:
            if (function_id == 0) {
                uint32_t mask = frame->regs[REG_A0];
                uint32_t base = frame->regs[REG_A1];
                uint32_t bit = 0;
                for (bit = 0; bit < 32; bit++) {
                    if (mask & (1u << bit)) {
                        sbi_send_ipi(base + bit);
                    }
                }
            } else {
                error = SBI_ERR_NOT_SUPPORTED;
            }
            break;

        case SBI_EXT_RFENCE:
            /*
             * There is one hart, so a remote fence is a local one. The
             * emulator flushes its translation cache on every fence, which is
             * what actually matters here.
             */
            switch (function_id)
            {
                case 0:
                    __asm__ volatile("fence.i" ::: "memory");
                    break;
                case 1:
                case 2:
                    __asm__ volatile("sfence.vma" ::: "memory");
                    break;
                default:
                    error = SBI_ERR_NOT_SUPPORTED;
                    break;
            }
            break;

        case SBI_EXT_SRST:
            if (function_id == 0) {
                sbi_shutdown();
            } else {
                error = SBI_ERR_NOT_SUPPORTED;
            }
            break;

        default:
            error = SBI_ERR_NOT_SUPPORTED;
            break;
    }

    frame->regs[REG_A0] = (uint32_t)error;
    frame->regs[REG_A1] = (uint32_t)value;
}

static void
sbi_dispatch(struct trap_frame * frame)
{
    uint32_t extension_id = frame->regs[REG_A7];
    uint32_t function_id = frame->regs[REG_A6];

    if (extension_id <= SBI_EXT_LEGACY_SHUTDOWN) {
        sbi_legacy_call(frame, extension_id);
    } else {
        sbi_modern_call(frame, extension_id, function_id);
    }
}

static void
fw_fatal(const char * what, uint32_t mcause, uint32_t mepc, uint32_t mtval)
{
    printk("\r\n[firmware] unhandled %s\r\n", what);
    printk("[firmware]   mcause 0x%x mepc 0x%x mtval 0x%x\r\n",
           mcause, mepc, mtval);
    printk("[firmware] halting.\r\n");
    for (;;) {
        __asm__ volatile("wfi");
    }
}

static void
fw_handle_interrupt(uint32_t cause)
{
    switch (cause)
    {
        case IRQ_MACHINE_TIMER:
            /*
             * Hand the tick up to the supervisor. mtimecmp cannot be cleared
             * without a new deadline, so silence the machine timer by
             * disabling it; the next sbi_set_timer re-enables it.
             */
            clear_csr(mie, MIE_MTIE);
            set_csr(mip, MIP_STIP);
            break;
        case IRQ_MACHINE_SOFTWARE:
            {
                volatile uint32_t * msip =
                    (volatile uint32_t *)(CLINT_BASE + CLINT_MSIP_OFFSET +
                                          4 * current_hartid());
                *msip = 0;
                set_csr(mip, MIP_SSIP);
            }
            break;
        default:
            fw_fatal("machine interrupt", cause, read_csr(mepc), 0);
            break;
    }
}

void
fw_trap_handler(struct trap_frame * frame)
{
    uint32_t mcause = read_csr(mcause);

    if (mcause & 0x80000000u) {
        fw_handle_interrupt(mcause & 0xff);
        return;
    }

    switch (mcause)
    {
        case CAUSE_SUPERVISOR_ECALL:
            sbi_dispatch(frame);
            // Resume after the ecall rather than repeating it.
            write_csr(mepc, read_csr(mepc) + 4);
            break;
        default:
            fw_fatal("machine exception", mcause, read_csr(mepc),
                     read_csr(mtval));
            break;
    }
}

void
sbi_init(void)
{
    write_csr(mtvec, (unsigned long)fw_trap_vector);
    // Only the timer and the software interrupt are ours; everything the
    // platform controller raises is delegated straight to the supervisor.
    write_csr(mie, MIE_MSIE);
    write_csr(mip, 0);
}
