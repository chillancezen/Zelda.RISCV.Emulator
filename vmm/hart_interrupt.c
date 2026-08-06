/*
 * Copyright (c) 2020 Jie Zheng
 */
#include <hart_interrupt.h>
#include <clint.h>
#include <plic.h>
#include <uart.h>
#include <unistd.h>

/*
 * How much guest time one translation unit is accounted for. The guest clock
 * is virtual (see clint.h), so this constant alone decides the ratio between
 * emulated instructions and emulated seconds. Raising it makes the guest
 * clock run faster relative to the work it gets done, which shortens boot in
 * wall-clock terms but leaves the guest less real time per timer tick.
 */
static uint64_t ticks_per_translation_unit = 2;

void
set_ticks_per_translation_unit(uint64_t ticks)
{
    ASSERT(ticks > 0);
    ticks_per_translation_unit = ticks;
}

uint8_t
interrupt_target_privilege_level(struct hart * hartptr, uint8_t vector)
{
    uint8_t target_privilege_level = PRIVILEGE_LEVEL_MACHINE;
    ASSERT(vector < 12);
    if (hartptr->idelegation.dword & (1 << vector)) {
        target_privilege_level = PRIVILEGE_LEVEL_SUPERVISOR;
    }
    return target_privilege_level;
}

uint8_t
is_interrupt_deliverable(struct hart * hartptr, uint8_t vector)
{
    uint8_t target_pl = interrupt_target_privilege_level(hartptr, vector);
    uint8_t current_pl = hartptr->privilege_level;
    uint8_t globally_enabled;

    if (!(hartptr->ipending.dword & (1 << vector)) ||
        !(hartptr->ienable.dword & (1 << vector))) {
        return 0;
    }

    /*
     * An interrupt destined for privilege level x is taken when the hart runs
     * below x unconditionally, and when it runs at x only if that level has
     * interrupts globally enabled. Interrupts destined for a level below the
     * current one are never taken.
     */
    if (current_pl > target_pl) {
        return 0;
    }
    if (current_pl < target_pl) {
        return 1;
    }
    globally_enabled = (target_pl == PRIVILEGE_LEVEL_MACHINE) ?
                       hartptr->status.mie : hartptr->status.sie;
    return globally_enabled ? 1 : 0;
}

void
deliver_interrupt(struct hart * hartptr, uint8_t vector)
{
    // No need to check whether the interrupt is deliverable.
    // the caller should make sure the interrupt window open and then call this
    // to deliver the interrupt.
    uint8_t target_pl = interrupt_target_privilege_level(hartptr, vector);
    raise_trap_raw(hartptr, target_pl, 1 << 31 | vector, 0);
}

/*
 * Reading the host terminal costs a system call, and this runs at every
 * translation unit boundary - often enough that polling each time would cost
 * more than executing the guest. A console types at human speed, so checking
 * a few thousand times a second is still far faster than anyone can type.
 */
#define CONSOLE_POLL_INTERVAL 4096

/*
 * Refresh the pending bits every device owns. Software-written bits such as
 * STIP are left alone: only the hardware sources are recomputed here.
 */
static void
refresh_device_interrupts(struct hart * hartptr)
{
    static uint32_t poll_countdown = 0;

    if (!poll_countdown--) {
        poll_countdown = CONSOLE_POLL_INTERVAL;
        uart_poll_input();
    }
    uart_refresh_interrupt();
    clint_refresh_pending(hartptr);
    plic_refresh_pending(hartptr);
}

/*
 * Advance the guest clock, let the devices update their pending state, and
 * take the highest priority interrupt that is ready.
 *
 * This runs at every translation unit boundary, which is the only point where
 * the vmm reliably holds control while the guest is between instructions.
 * If an interrupt is taken this never returns: raise_trap_raw() switches to
 * the trap vector and re-enters the translation loop from there.
 */
void
pump_devices_and_deliver_interrupts(struct hart * hartptr)
{
    /*
     * Machine interrupts outrank supervisor ones, and within a level external
     * outranks software which outranks timer.
     */
    static const uint8_t delivery_order[] = {
        INTERRUPT_MACHINE_EXTERNAL,
        INTERRUPT_MACHINE_SOFTWARE,
        INTERRUPT_MACHINE_TIMER,
        INTERRUPT_SUPERVISOR_EXTERNAL,
        INTERRUPT_SUPERVISOR_SOFTWARE,
        INTERRUPT_SUPERVISOR_TIMER
    };
    int idx = 0;

    clint_advance_time(ticks_per_translation_unit);
    refresh_device_interrupts(hartptr);

    for (idx = 0; idx < (int)(sizeof(delivery_order)/sizeof(delivery_order[0]));
         idx++) {
        if (is_interrupt_deliverable(hartptr, delivery_order[idx])) {
            deliver_interrupt(hartptr, delivery_order[idx]);
            __not_reach();
        }
    }
}

/*
 * Called when the guest executes wfi. There is nothing else running on the
 * host that could make progress on the guest's behalf, so rather than spin we
 * fast-forward the clock to whenever the next timer is due. If no timer is
 * armed the only thing that can wake the hart is a device, so poll for one.
 */
void
hart_idle(struct hart * hartptr)
{
    int idx = 0;

    for (idx = 0; idx < 64; idx++) {
        // An idle hart is exactly when the console does deserve a real poll.
        uart_poll_input();
        refresh_device_interrupts(hartptr);
        if (hartptr->ipending.dword & hartptr->ienable.dword) {
            return;
        }
        if (!clint_skip_to_next_deadline()) {
            // Nothing is going to wake us from the clock. Give the host back
            // some cycles and look for console input again.
            usleep(1000);
        }
    }
}
