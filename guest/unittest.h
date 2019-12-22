/*
 * Copyright (c) 2019 Jie Zheng
 */

#ifndef _UNITTEST_H
#define _UNITTEST_H
#include <stdint.h>
#include <printk.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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


void
unit_test(void);
#endif
