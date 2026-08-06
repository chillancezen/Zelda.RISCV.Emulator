/*
 * Copyright (c) 2020 Jie Zheng
 */

#ifndef _SYSCON_H
#define _SYSCON_H

struct virtual_machine;
struct fdt_build_blob;

// Must match SYSCON_BASE in the firmware's platform.h.
#define SYSCON_BASE_DEFAULT 0x00400000

void
build_syscon_fdt_node(struct fdt_build_blob * blob);

void
syscon_init(struct virtual_machine * vm);

#endif
