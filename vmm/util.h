/*
 * Copyright (c) 2019 Jie Zheng
 */
#ifndef _UTIL_H
#define _UTIL_H
#include <stdint.h>
#include <sort.h>
#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <log.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static inline int32_t
sign_extend32(uint32_t data, int sign_bit)
{
    int32_t ret = data;
    int bit_position = 0;
    int32_t sign = (data >> sign_bit) & 1;
    for (bit_position = sign_bit; bit_position <= 31; bit_position++) {
        ret |= (sign << bit_position);
    }
    return ret;
}
#define ASSERT(exp)                                                            \
    if (!(exp)) {                                                              \
        log_fatal("assertion:%s fails\n", #exp);                               \
        exit(-1);                                                              \
    }

#define __NOT_REACH 0
#define __not_reach()                                                          \
    ASSERT(__NOT_REACH)


#define BREAKPOINT()                                                           \
    __asm__ volatile(".byte 0xcc")

#if defined(COLORED_OUTPUT)
    #define ANSI_COLOR_RED     "\x1b[31m"
    #define ANSI_COLOR_GREEN   "\x1b[32m"
    #define ANSI_COLOR_YELLOW  "\x1b[33m"
    #define ANSI_COLOR_BLUE    "\x1b[34m"
    #define ANSI_COLOR_MAGENTA "\x1b[35m"
    #define ANSI_COLOR_CYAN    "\x1b[36m"
    #define ANSI_COLOR_RESET   "\x1b[0m"

#else
    #define ANSI_COLOR_RED     ""
    #define ANSI_COLOR_GREEN   ""
    #define ANSI_COLOR_YELLOW  ""
    #define ANSI_COLOR_BLUE    ""
    #define ANSI_COLOR_MAGENTA ""
    #define ANSI_COLOR_CYAN    ""
    #define ANSI_COLOR_RESET   " "
#endif

#endif
