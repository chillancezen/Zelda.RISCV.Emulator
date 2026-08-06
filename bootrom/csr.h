/*
 * Copyright (c) 2020 Jie Zheng
 *
 *      Machine mode control and status register access.
 */

#ifndef _FW_CSR_H
#define _FW_CSR_H

#define read_csr(reg) ({                                                       \
    unsigned long __v;                                                         \
    __asm__ volatile("csrr %0, " #reg : "=r"(__v) : : "memory");               \
    __v;                                                                       \
})

#define write_csr(reg, val)                                                    \
    __asm__ volatile("csrw " #reg ", %0" : : "rK"((unsigned long)(val)) : "memory")

#define set_csr(reg, bits)                                                     \
    __asm__ volatile("csrs " #reg ", %0" : : "rK"((unsigned long)(bits)) : "memory")

#define clear_csr(reg, bits)                                                   \
    __asm__ volatile("csrc " #reg ", %0" : : "rK"((unsigned long)(bits)) : "memory")

// mstatus / mie / mip bit positions.
#define MSTATUS_SIE     (1u << 1)
#define MSTATUS_MIE     (1u << 3)
#define MSTATUS_SPIE    (1u << 5)
#define MSTATUS_MPIE    (1u << 7)
#define MSTATUS_SPP     (1u << 8)
#define MSTATUS_MPP_SHIFT 11
#define MSTATUS_MPP_MASK  (3u << MSTATUS_MPP_SHIFT)

#define MIP_SSIP        (1u << 1)
#define MIP_MSIP        (1u << 3)
#define MIP_STIP        (1u << 5)
#define MIP_MTIP        (1u << 7)
#define MIP_SEIP        (1u << 9)
#define MIP_MEIP        (1u << 11)

#define MIE_SSIE        MIP_SSIP
#define MIE_MSIE        MIP_MSIP
#define MIE_STIE        MIP_STIP
#define MIE_MTIE        MIP_MTIP
#define MIE_SEIE        MIP_SEIP
#define MIE_MEIE        MIP_MEIP

#define PRIVILEGE_SUPERVISOR 1
#define PRIVILEGE_MACHINE    3

// Exception causes.
#define CAUSE_MISALIGNED_FETCH      0x0
#define CAUSE_FETCH_ACCESS          0x1
#define CAUSE_ILLEGAL_INSTRUCTION   0x2
#define CAUSE_BREAKPOINT            0x3
#define CAUSE_MISALIGNED_LOAD       0x4
#define CAUSE_LOAD_ACCESS           0x5
#define CAUSE_MISALIGNED_STORE      0x6
#define CAUSE_STORE_ACCESS          0x7
#define CAUSE_USER_ECALL            0x8
#define CAUSE_SUPERVISOR_ECALL      0x9
#define CAUSE_MACHINE_ECALL         0xb
#define CAUSE_FETCH_PAGE_FAULT      0xc
#define CAUSE_LOAD_PAGE_FAULT       0xd
#define CAUSE_STORE_PAGE_FAULT      0xf

// Interrupt causes, with the top bit of mcause already stripped.
#define IRQ_SUPERVISOR_SOFTWARE     1
#define IRQ_MACHINE_SOFTWARE        3
#define IRQ_SUPERVISOR_TIMER        5
#define IRQ_MACHINE_TIMER           7
#define IRQ_SUPERVISOR_EXTERNAL     9
#define IRQ_MACHINE_EXTERNAL        11

#endif
