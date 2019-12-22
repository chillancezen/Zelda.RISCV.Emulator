/*
 * Copyright (c) 2019 Jie Zheng
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdint.h>
#include <hart.h>
void
enter_vmm_shell(struct hart * hartptr);


int
add_breakpoint(uint32_t guest_addr);

int
is_address_breakpoint(uint32_t guest_addr);

void
dump_breakpoints(void);
#endif
