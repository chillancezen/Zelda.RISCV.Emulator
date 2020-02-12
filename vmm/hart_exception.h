/*
 * Copyright (c) 2020 Jie Zheng
 *
 *
 *      Hart Exception is a wrapper layer for exception delivery in hart
 */

#ifndef _HART_EXCEPTION_H
#define _HART_EXCEPTION_H
#include <hart_trap.h>

#define EXCEPTION_INSTRUCTION_ADDRESS_MISALIGN  0x0
#define EXCEPTION_INSTRUCTION_ACCESS_FAULT      0x1
#define EXCEPTION_ILLEEGAL_INSTRUCTION          0x2
#define EXCEPTION_BREAKPOINT                    0x3
#define EXCEPTION_LOAD_ADDRESS_MISALIGN         0x4
#define EXCEPTION_LOAD_ACCESS_FAULT             0x5
#define EXCEPTION_STORE_ADDRESS_MISALIGN        0x6
#define EXCEPTION_STORE_ACCESS_FAULT            0x7
#define EXCEPTION_ECALL_FROM_UMODE              0x8
#define EXCEPTION_ECALL_FROM_SMODE              0x9
#define EXCEPTION_ECALL_FROM_MMODE              0xb
#define EXCEPTION_INSTRUCTION_PAGE_FAULT        0xc
#define EXCEPTION_LOAD_PAGE_FAULT               0xd
#define EXCEPTION_STORE_PAGE_FAULT              0xf

void
raise_exception(struct hart * hartptr, uint8_t exception_cause);

#endif
