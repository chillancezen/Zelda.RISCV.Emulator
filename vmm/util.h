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
int
preload_image(void * addr, int64_t length, const char * image_path);

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
        printf("[assert] %s:%d assertion:%s failed\n",                         \
               __FILE__, __LINE__, #exp);                                      \
        exit(-1);                                                              \
    }

#define __NOT_REACH 0
#define __not_reach()                                                         \
    ASSERT(__NOT_REACH)


#define BREAKPOINT()                                                           \
    __asm__ volatile(".byte 0xcc")
#endif
