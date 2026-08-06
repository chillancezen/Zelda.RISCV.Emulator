/*
 * Copyright (c) 2020 Jie Zheng
 *
 *      Platform-Level Interrupt Controller: routes device interrupts to the
 *      external-interrupt line of each hart's local interrupt controller.
 */

#ifndef _PLIC_H
#define _PLIC_H
#include <stdint.h>

struct hart;
struct virtual_machine;
struct fdt_build_blob;

#define PLIC_NR_SOURCES 32

// Interrupt source numbers wired on this platform. Source 0 is reserved by
// the PLIC specification to mean "no interrupt".
#define PLIC_IRQ_UART0 10

/*
 * Each hart owns two PLIC contexts, matching the layout Linux expects from
 * the riscv,plic0 binding: an M-mode context followed by an S-mode context.
 */
#define PLIC_CONTEXTS_PER_HART 2
#define PLIC_CONTEXT_MACHINE(hart_id) ((hart_id) * PLIC_CONTEXTS_PER_HART + 0)
#define PLIC_CONTEXT_SUPERVISOR(hart_id) ((hart_id) * PLIC_CONTEXTS_PER_HART + 1)

/*
 * Raise or lower a device interrupt line. Level triggered: the device keeps
 * the line asserted until its own condition goes away.
 */
void
plic_set_pending(int source, int asserted);

/*
 * Recompute MEIP/SEIP for this hart from the PLIC state.
 */
void
plic_refresh_pending(struct hart * hartptr);

void
build_plic_fdt_node(struct fdt_build_blob * blob);

/*
 * The phandle devices point their interrupt-parent at. Only valid once
 * plic_reserve_phandle() has run, which happens before any device node is
 * emitted.
 */
int
plic_phandle(void);

void
plic_reserve_phandle(struct fdt_build_blob * blob);

void
plic_init(struct virtual_machine * vm);

#endif
