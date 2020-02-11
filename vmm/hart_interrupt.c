/*
 * Copyright (c) 2020 Jie Zheng
 */
#include <hart_interrupt.h>


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
    return (current_pl <= target_pl) &&
           (hartptr->ipending.dword & (1 << vector)) &&
           (hartptr->ienable.dword & (1 << vector)) &&
           (((target_pl == PRIVILEGE_LEVEL_MACHINE) && hartptr->status.mie) ||
            ((target_pl == PRIVILEGE_LEVEL_SUPERVISOR) && hartptr->status.sie));
}

void
deliver_interrupt(struct hart * hartptr, uint8_t vector)
{
    // No need to check whether the interrupt is deliverable.
    // the caller should make sure the interrupt window open and then call this
    // to deliver the interrupt.
    //ASSERT(is_interrupt_deliverable(hartptr, vector));
    uint8_t target_pl = interrupt_target_privilege_level(hartptr, vector);
    raise_trap_raw(hartptr, target_pl, 1 << 31 | vector, 0);
}
