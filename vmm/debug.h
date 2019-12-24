/*
 * Copyright (c) 2019 Jie Zheng
 */

#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdint.h>
#include <hart.h>

enum ACTIONS {
    ACTION_CONTINUE,
    ACTION_STOP
};

void
enter_vmm_dbg_shell(struct hart * hartptr, int check_bps);


int
add_breakpoint(uint32_t guest_addr);

int
is_address_breakpoint(uint32_t guest_addr);

void
dump_breakpoints(void);

int
add_breakpoint_command(struct hart * hartptr, int argc, char *argv[]);

#endif
