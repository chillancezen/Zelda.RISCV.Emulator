/*
 * Copyright (c) 2020 Jie Zheng
 *
 *      The supervisor binary interface this firmware presents to the kernel.
 */

#ifndef _SBI_H
#define _SBI_H
#include <stdint.h>

/*
 * Register save area laid down by fw_trap_vector, indexed by register number
 * so that frame->regs[10] is a0. Slot 0 is never written: x0 is hardwired.
 */
struct trap_frame {
    uint32_t regs[32];
};

#define REG_RA  1
#define REG_SP  2
#define REG_A0 10
#define REG_A1 11
#define REG_A2 12
#define REG_A3 13
#define REG_A4 14
#define REG_A5 15
#define REG_A6 16
#define REG_A7 17

// Extension ids. The four character ones are the ascii of their names.
#define SBI_EXT_LEGACY_SET_TIMER            0x00
#define SBI_EXT_LEGACY_CONSOLE_PUTCHAR      0x01
#define SBI_EXT_LEGACY_CONSOLE_GETCHAR      0x02
#define SBI_EXT_LEGACY_CLEAR_IPI            0x03
#define SBI_EXT_LEGACY_SEND_IPI             0x04
#define SBI_EXT_LEGACY_REMOTE_FENCE_I       0x05
#define SBI_EXT_LEGACY_REMOTE_SFENCE_VMA    0x06
#define SBI_EXT_LEGACY_REMOTE_SFENCE_VMA_ASID 0x07
#define SBI_EXT_LEGACY_SHUTDOWN             0x08

#define SBI_EXT_BASE                        0x10
#define SBI_EXT_TIME                        0x54494d45
#define SBI_EXT_IPI                         0x00735049
#define SBI_EXT_RFENCE                      0x52464e43
#define SBI_EXT_HSM                         0x0048534d
#define SBI_EXT_SRST                        0x53525354

#define SBI_BASE_GET_SPEC_VERSION           0x0
#define SBI_BASE_GET_IMPL_ID                0x1
#define SBI_BASE_GET_IMPL_VERSION           0x2
#define SBI_BASE_PROBE_EXTENSION            0x3
#define SBI_BASE_GET_MVENDORID              0x4
#define SBI_BASE_GET_MARCHID                0x5
#define SBI_BASE_GET_MIMPID                 0x6

#define SBI_SUCCESS                          0
#define SBI_ERR_FAILED                      (-1)
#define SBI_ERR_NOT_SUPPORTED               (-2)
#define SBI_ERR_INVALID_PARAM               (-3)

// (major << 24) | minor, per the specification's version encoding.
#define SBI_SPEC_VERSION                    ((0 << 24) | 3)
// 8 is the id the specification reserves for "unknown implementation".
#define SBI_IMPL_ID                         8
#define SBI_IMPL_VERSION                    1

void
fw_trap_handler(struct trap_frame * frame);

void
sbi_init(void);

// Defined in entry.S.
void
fw_trap_vector(void);

void
enter_supervisor(uint32_t hartid, uint32_t dtb, uint32_t entry);

#endif
