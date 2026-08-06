/*
 * Copyright (c) 2020 Jie Zheng
 *
 *
 *      Hart Interrupt is a wrapper layer for interrupt delivery in hart
 */

#ifndef _HART_INTERRUPT_H
#define _HART_INTERRUPT_H
#include <hart_trap.h>

#define INTERRUPT_USER_SOFTWARE         0x0
#define INTERRUPT_SUPERVISOR_SOFTWARE   0x1
#define INTERRUPT_MACHINE_SOFTWARE      0x3
#define INTERRUPT_USER_TIMER            0x4
#define INTERRUPT_SUPERVISOR_TIMER      0x5
#define INTERRUPT_MACHINE_TIMER         0x7
#define INTERRUPT_USER_EXTERNAL         0x8
#define INTERRUPT_SUPERVISOR_EXTERNAL   0x9
#define INTERRUPT_MACHINE_EXTERNAL      0xb


uint8_t
is_interrupt_deliverable(struct hart * hartptr, uint8_t vector);

void
deliver_interrupt(struct hart * hartptr, uint8_t vector);

/*
 * Advance the guest clock, refresh device state and take any interrupt that
 * has become deliverable. Does not return if an interrupt is taken.
 */
void
pump_devices_and_deliver_interrupts(struct hart * hartptr);

/*
 * Idle the hart until something can wake it, used to implement wfi.
 */
void
hart_idle(struct hart * hartptr);

void
set_ticks_per_translation_unit(uint64_t ticks);

#endif

