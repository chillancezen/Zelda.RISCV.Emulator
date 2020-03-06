/*
 * Copyright (c) 2020 Jie Zheng
 */
#include <hart_exception.h>
#include <hart_trap.h>
#include <csr.h>
#include <string.h>
/*
 * exception is one kind of synchronous trap.
 * the exception trap is finally taken at the privilege level by following the
 * selection logic:
 *  1). exception occurs in m-mode, trap taken in m-mode.
 *  2). exception occurs in s-mode, the exception is not delegated, trap taken in m-mode.
 *  3). exception occurs in s-mode, the exception is delegated, trap taken in s-mode.
 *  4). exception occurs in u-mode, the exception is not delegated, trap taken in m-mdoe.
 *  5). exception occurs in u-mode, the exception is delegated, trap taken in s-mode.
 */
static uint8_t
exception_target_privilege_level(struct hart * hartptr, uint8_t exception_number)
{
    uint8_t current_pl = hartptr->privilege_level;
    uint8_t target_pl = PRIVILEGE_LEVEL_MACHINE;
    struct csr_entry * csr_medeleg =
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_MEDELEG];
    if ((current_pl != PRIVILEGE_LEVEL_MACHINE) &&
        (csr_medeleg->csr_blob & (1 << exception_number))) {
        target_pl = PRIVILEGE_LEVEL_SUPERVISOR;
    }
    return target_pl;
}


static uint8_t trap_value_map[16];

void
raise_exception_internal(struct hart * hartptr, uint8_t exception_cause,
                         uint32_t trap_value)
{
    uint8_t target_pl =
        exception_target_privilege_level(hartptr, exception_cause);
    ASSERT(exception_cause < 16); 
    raise_trap_raw(hartptr, target_pl, exception_cause, trap_value);

}

__attribute__((unused)) void
trap_value_map_init(void)
{
    memset(trap_value_map, 0x0, sizeof(trap_value_map));
    trap_value_map[EXCEPTION_BREAKPOINT] = 1;
    trap_value_map[EXCEPTION_INSTRUCTION_ADDRESS_MISALIGN] = 1;
    trap_value_map[EXCEPTION_INSTRUCTION_ACCESS_FAULT] = 1;
    trap_value_map[EXCEPTION_ILLEEGAL_INSTRUCTION] = 1;
    trap_value_map[EXCEPTION_LOAD_ADDRESS_MISALIGN] = 1;
    trap_value_map[EXCEPTION_LOAD_ACCESS_FAULT] = 1;
    trap_value_map[EXCEPTION_STORE_ADDRESS_MISALIGN] = 1;
    trap_value_map[EXCEPTION_STORE_ACCESS_FAULT] = 1;
    trap_value_map[EXCEPTION_INSTRUCTION_PAGE_FAULT] = 1;
    trap_value_map[EXCEPTION_LOAD_PAGE_FAULT] = 1;
    trap_value_map[EXCEPTION_STORE_PAGE_FAULT] = 1;
}
