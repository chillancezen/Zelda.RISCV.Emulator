/*
 * Copyright (c) 2019-2020 Jie Zheng
 *
 *      Machine mode entry points: the reset vector and the trap vector.
 */

#define FW_STACK_SHIFT 13               /* 8KiB of machine mode stack per hart */
#define FW_STACK_SIZE  (1 << FW_STACK_SHIFT)

.section .rom_init_entry
.global rom_entry
/*
 * The firmware is linked at a fixed address, so symbols are reached with
 * absolute hi/lo pairs. The pc-relative form that `la` expands to sends the
 * linker down its relaxation path, which it cannot complete for a symbol
 * sitting at the end of .bss.
 */
#define LOAD_ADDRESS(reg, sym)                                                 \
    lui   reg, %hi(sym);                                                       \
    addi  reg, reg, %lo(sym)

rom_entry:
    /* Give every hart its own slice of the machine mode stack. */
    csrr  a0, mhartid
    LOAD_ADDRESS(t0, __fw_stack_top)
    slli  t1, a0, FW_STACK_SHIFT
    sub   sp, t0, t1

    /* The rom is plain memory that nobody cleared for us. */
    LOAD_ADDRESS(t0, __bss_start)
    LOAD_ADDRESS(t1, __bss_end)
1:
    bgeu  t0, t1, 2f
    sw    zero, 0(t0)
    addi  t0, t0, 4
    j     1b
2:
    .extern fw_main
    jal   ra, fw_main

    /* fw_main enters the kernel and never comes back. */
3:
    wfi
    j     3b

/*
 * Traps taken in machine mode. The supervisor's sp is a virtual address and
 * machine mode runs unpaged, so the very first thing we do is swap it for our
 * own stack, which mscratch holds while the guest is running.
 */
.section .text
.align 4
.global fw_trap_vector
fw_trap_vector:
    csrrw sp, mscratch, sp
    addi  sp, sp, -32*4

    sw    x1,   1*4(sp)
    sw    x3,   3*4(sp)
    sw    x4,   4*4(sp)
    sw    x5,   5*4(sp)
    sw    x6,   6*4(sp)
    sw    x7,   7*4(sp)
    sw    x8,   8*4(sp)
    sw    x9,   9*4(sp)
    sw    x10, 10*4(sp)
    sw    x11, 11*4(sp)
    sw    x12, 12*4(sp)
    sw    x13, 13*4(sp)
    sw    x14, 14*4(sp)
    sw    x15, 15*4(sp)
    sw    x16, 16*4(sp)
    sw    x17, 17*4(sp)
    sw    x18, 18*4(sp)
    sw    x19, 19*4(sp)
    sw    x20, 20*4(sp)
    sw    x21, 21*4(sp)
    sw    x22, 22*4(sp)
    sw    x23, 23*4(sp)
    sw    x24, 24*4(sp)
    sw    x25, 25*4(sp)
    sw    x26, 26*4(sp)
    sw    x27, 27*4(sp)
    sw    x28, 28*4(sp)
    sw    x29, 29*4(sp)
    sw    x30, 30*4(sp)
    sw    x31, 31*4(sp)

    /* The interrupted sp is parked in mscratch; record it in the frame. */
    csrr  t0, mscratch
    sw    t0,  2*4(sp)
    /* Leave mscratch pointing at our stack again so a nested trap works. */
    addi  t0, sp, 32*4
    csrw  mscratch, t0

    .extern fw_trap_handler
    mv    a0, sp
    jal   ra, fw_trap_handler

    /* Restore the interrupted sp through mscratch. */
    lw    t0,  2*4(sp)
    csrw  mscratch, t0

    lw    x1,   1*4(sp)
    lw    x3,   3*4(sp)
    lw    x4,   4*4(sp)
    lw    x5,   5*4(sp)
    lw    x6,   6*4(sp)
    lw    x7,   7*4(sp)
    lw    x8,   8*4(sp)
    lw    x9,   9*4(sp)
    lw    x10, 10*4(sp)
    lw    x11, 11*4(sp)
    lw    x12, 12*4(sp)
    lw    x13, 13*4(sp)
    lw    x14, 14*4(sp)
    lw    x15, 15*4(sp)
    lw    x16, 16*4(sp)
    lw    x17, 17*4(sp)
    lw    x18, 18*4(sp)
    lw    x19, 19*4(sp)
    lw    x20, 20*4(sp)
    lw    x21, 21*4(sp)
    lw    x22, 22*4(sp)
    lw    x23, 23*4(sp)
    lw    x24, 24*4(sp)
    lw    x25, 25*4(sp)
    lw    x26, 26*4(sp)
    lw    x27, 27*4(sp)
    lw    x28, 28*4(sp)
    lw    x29, 29*4(sp)
    lw    x30, 30*4(sp)
    lw    x31, 31*4(sp)

    addi  sp, sp, 32*4
    csrrw sp, mscratch, sp
    mret

/*
 * enter_supervisor(hartid, dtb, entry)
 *
 * Hand control to the kernel. mret drops us to the privilege level parked in
 * mstatus.MPP, which fw_main has already set to supervisor.
 */
.global enter_supervisor
enter_supervisor:
    csrw  mepc, a2
    /* Our stack top becomes the machine mode trap stack from here on. */
    LOAD_ADDRESS(t0, __fw_stack_top)
    csrr  t1, mhartid
    slli  t1, t1, FW_STACK_SHIFT
    sub   t0, t0, t1
    csrw  mscratch, t0
    /* a0 and a1 are already the hart id and the device tree address. */
    mret

.section .bss
.align 16
.global __fw_stack_bottom
__fw_stack_bottom:
    .space FW_STACK_SIZE * 4
.global __fw_stack_top
__fw_stack_top:
    /* Padding so the stack top symbol is not the last byte of the section. */
    .space 16
