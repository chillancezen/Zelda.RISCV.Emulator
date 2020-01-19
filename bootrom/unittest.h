/*
 * Copyright (c) 2019 Jie Zheng
 */

#ifndef _UNITTEST_H
#define _UNITTEST_H
#include <stdint.h>
#include <printk.h>

#define TEST_R_R_R(intr, rd, rs1, rs2)                                         \
{                                                                              \
    uint32_t __output_operand = 0;                                             \
    __asm__ volatile(#intr " %[RD], %[RS1], %[RS2];"                           \
                     :[RD]"=r"(__output_operand)                               \
                     :[RS1]"r"(rs1), [RS2]"r"(rs2)                             \
                     :"memory");                                               \
    if (__output_operand == rd) {                                              \
        printk(ANSI_COLOR_GREEN"PASS"ANSI_COLOR_RESET" instruction [" #intr " "\
                #rd ", " #rs1 ", " #rs2 "]\n");                                \
    } else {                                                                   \
        printk(ANSI_COLOR_RED"FAIL"ANSI_COLOR_RESET" instruction [" #intr " "  \
               #rd ", " #rs1 ", " #rs2 "]\n");                                 \
    }                                                                          \
}


#define TEST_R_R_I(intr, rd, rs1, imm)                                         \
{                                                                              \
    uint32_t __output_operand = 0;                                             \
    __asm__ volatile(#intr " %[RD], %[RS1], " #imm ";"                         \
                     :[RD]"=r"(__output_operand)                               \
                     :[RS1]"r"(rs1)                                            \
                     :"memory");                                               \
    if (__output_operand == rd) {                                              \
        printk(ANSI_COLOR_GREEN"PASS"ANSI_COLOR_RESET" instruction [" #intr " "\
                #rd ", " #rs1 ", " #imm "]\n");                                \
    } else {                                                                   \
        printk(ANSI_COLOR_RED"FAIL"ANSI_COLOR_RESET" instruction [" #intr " "  \
               #rd ", " #rs1 ", " #imm "]\n");                                 \
    }                                                                          \
}


#define MEM_LOAD_COMMON(instr, addr, ret)                                      \
        __asm__ volatile(#instr " %[RET], (%[ADDR]);"                          \
                         :[RET]"=r"(ret)                                       \
                         :[ADDR]"r"(addr)                                      \
                         :"memory")

#define LB(addr, ret)   MEM_LOAD_COMMON(lb, addr, ret)
#define LBU(addr, ret)  MEM_LOAD_COMMON(lbu, addr, ret)
#define LH(addr, ret)   MEM_LOAD_COMMON(lh, addr, ret)
#define LHU(addr, ret)  MEM_LOAD_COMMON(lhu, addr, ret)
#define LW(addr, ret)   MEM_LOAD_COMMON(lw, addr, ret)

#define MEM_STORE_COMMON(instr, addr, value)                                   \
        __asm__ volatile(#instr " %[VALUE], (%[ADDR]);"                        \
                         :                                                     \
                         :[ADDR]"r"(addr), [VALUE]"r"(value)                   \
                         :"memory")

#define SB(addr, value) MEM_STORE_COMMON(sb, addr, value)
#define SH(addr, value) MEM_STORE_COMMON(sh, addr, value)
#define SW(addr, value) MEM_STORE_COMMON(sw, addr, value)

void
unit_test(void);
#endif
