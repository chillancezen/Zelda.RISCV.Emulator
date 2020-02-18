/*
 * Copyright (c) 2020 Jie Zheng
 */

#ifndef _HART_DEF_H
#define _HART_DEF_H

#if XLEN == 32
#define REGISTER_TYPE uint32_t
#elif XLEN == 64
#define REGISTER_TYPE uint64_t
#endif


// XXX: make it big, so it doesn't need to be flushed when debuging the TC
#define TRANSLATION_CACHE_SIZE (1024 * 64)
// XXX: make it not that big, because it takes too much to search translated instruction.
#define MAX_INSTRUCTIONS_TOTRANSLATE 512

// reserve a small trunk of space to transfer control to vmm
#define VMM_STACK_SIZE (1024 * 8)

// for debug reason, put a magic word in each hart.
#define HART_MAGIC_WORD 0xdeadbeef

#define PRIVILEGE_LEVEL_USER 0x0
#define PRIVILEGE_LEVEL_SUPERVISOR 0x1
#define PRIVILEGE_LEVEL_RESERVED 0x2
#define PRIVILEGE_LEVEL_MACHINE 0x3



#define CSR_ADDRESS_MSCRATCH            0x340
#define CSR_ADDRESS_MEPC                0x341
#define CSR_ADDRESS_MSTATUS             0x300
#define CSR_ADDRESS_MIDELEG             0x303
#define CSR_ADDRESS_MEDELEG             0x302
#define CSR_ADDRESS_MIP                 0x344
#define CSR_ADDRESS_MIE                 0x304
#define CSR_ADDRESS_MCAUSE              0x342
#define CSR_ADDRESS_MTVAL               0x343
#define CSR_ADDRESS_MTVEC               0x305


#define CSR_ADDRESS_SIE                 0x104
#define CSR_ADDRESS_SIP                 0x144
#define CSR_ADDRESS_SSTATUS             0x100
#define CSR_ADDRESS_STVEC               0x105
#define CSR_ADDRESS_SATP                0x180

#define CSR_ADDRESS_SCAUSE              0x142
#define CSR_ADDRESS_STVAL               0x143
#define CSR_ADDRESS_SEPC                0x141

#endif
