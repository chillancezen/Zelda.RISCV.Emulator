/*
 * Copyright (c) 2020 Jie Zheng
 */
#ifndef _HART_TRAP_H
#define _HART_TRAP_H
#include <stdint.h>
#include <hart.h>

void
raise_trap_raw(struct hart * hartptr, uint8_t target_privilege_level,
               uint32_t cause, uint32_t tval);

#endif
