/*
 * Copyright (c) 2020 Jie Zheng
 *
 *      Core Local Interruptor: the machine timer (mtime/mtimecmp) and the
 *      software interrupt (msip) doorbell.
 */

#ifndef _CLINT_H
#define _CLINT_H
#include <stdint.h>

struct hart;
struct virtual_machine;
struct fdt_build_blob;

/*
 * The guest clock is virtual: it is driven by how much guest code has actually
 * been executed rather than by the host wall clock.  Tying it to the host
 * instead would make the guest observe a machine that executes a handful of
 * instructions per timer tick, and the kernel would spend its entire life
 * inside the timer interrupt handler.  A virtual clock keeps the ratio of
 * "guest cycles executed" to "guest time elapsed" constant, so the guest sees
 * a slow but self-consistent machine.
 */
#define CLINT_TIMEBASE_FREQUENCY 10000000

// Register offsets, matching the SiFive CLINT that riscv,clint0 describes.
#define CLINT_REG_MSIP          0x0000
#define CLINT_REG_MTIMECMP      0x4000
#define CLINT_REG_MTIME         0xbff8

/*
 * Advance the guest clock. Called once per translation unit boundary with the
 * number of ticks that unit is accounted for.
 */
void
clint_advance_time(uint64_t ticks);

/*
 * Jump the clock straight to the earliest armed mtimecmp. Used to implement
 * wfi so an idle guest does not burn host cycles waiting for its own timer.
 * Returns 0 if no timer is armed and hence nothing was skipped.
 */
int
clint_skip_to_next_deadline(void);

uint64_t
clint_get_time(void);

/*
 * Refresh MTIP/MSIP in the hart's pending-interrupt register from the current
 * clock and doorbell state.
 */
void
clint_refresh_pending(struct hart * hartptr);

void
build_clint_fdt_node(struct fdt_build_blob * blob);

void
clint_init(struct virtual_machine * vm);

#endif
