/*
 * Copyright (c) 2019 Jie Zheng
 */
#ifndef _HART_H
#define _HART_H

#include <stdint.h>


#define XLEN 32

#if XLEN == 32
#define REGISTER_TYPE uint32_t
#elif XLEN == 64
#define REGISTER_TYPE uint64_t
#endif

#define DEFAULT_TRANSLATION_CACHE_SIZE (4096 * 1)

struct integer_register_profile {
    REGISTER_TYPE zero;
    REGISTER_TYPE ra;
    REGISTER_TYPE sp;
    REGISTER_TYPE gp;
    REGISTER_TYPE tp;
    REGISTER_TYPE t0, t1, t2;
    REGISTER_TYPE s0, s1;
    REGISTER_TYPE a0, a1, a2, a3, a4, a5, a6, a7;
    REGISTER_TYPE s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
    REGISTER_TYPE t3, t4, t5, t6;
}__attribute__((packed));



struct virtual_machine;

struct hart {
    struct integer_register_profile registers __attribute__((aligned(64)));
    int hart_id;
    uint32_t pc;
    struct virtual_machine * vmptr;

    void * translation_cache;
    int translation_cache_size;
}__attribute__((aligned(64)));



void
hart_init(struct hart * hart_instance, int hart_id);
#endif
