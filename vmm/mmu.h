/*
 * Copyright (c) 2019 Jie Zheng
 */
#ifndef _MMU_H
#define _MMU_H
#include <vm.h>
#include <hart.h>
#include <pm_region.h>
#include <csr.h>
#include <mmu_tlb.h>

#define _(_size)                                                               \
__attribute__((always_inline)) static inline uint##_size##_t                   \
direct_read##_size (struct hart * hartptr, uint32_t linear_address)            \
{                                                                              \
    struct pm_region_operation * pmr;                                          \
    pmr = search_pm_region_callback(linear_address);                           \
    if (!pmr) {                                                                \
        dump_hart(hartptr);                                                    \
        __not_reach();                                                         \
    }                                                                          \
    return pmr->pmr_read(linear_address, _size / 8, hartptr, pmr);             \
}



_(8)
_(16)
_(32)
_(64)
#undef _


#define _(_size)                                                               \
__attribute__((always_inline)) static inline void                              \
direct_write##_size (struct hart * hartptr, uint32_t linear_address,           \
                     uint##_size##_t value)                                    \
{                                                                              \
    struct pm_region_operation * pmr;                                          \
    pmr = search_pm_region_callback(linear_address);                           \
    if (!pmr) {                                                                \
        dump_hart(hartptr);                                                    \
        __not_reach();                                                         \
    }                                                                          \
    pmr->pmr_write(linear_address, _size / 8, value, hartptr, pmr);            \
}



_(8)
_(16)
_(32)
_(64)
#undef _

int
walk_page_table(struct hart * hartptr, uint32_t va, struct tlb_entry * tlb,
                int tlb_cap);

#include <hart_exception.h>

// FIXME: raise exception if address is not naturally aligned.
#define _(_size)                                                               \
__attribute__((always_inline)) static inline uint##_size##_t                   \
vmread##_size (struct hart * hartptr, uint32_t linear_address)                 \
{                                                                              \
    struct csr_entry * csr =                                                   \
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_SATP];           \
    if (hartptr->privilege_level < PRIVILEGE_LEVEL_MACHINE &&                  \
        csr->csr_blob & 0x80000000) {                                          \
        struct tlb_entry * entry = VA_TO_TLB_ENTRY(hartptr->dtlb,              \
                                                   hartptr->dtlb_cap,          \
                                                   linear_address);            \
        if (!entry) {                                                          \
            walk_page_table(hartptr, linear_address, hartptr->dtlb,            \
                            hartptr->dtlb_cap);                                \
            entry = VA_TO_TLB_ENTRY(hartptr->dtlb, hartptr->dtlb_cap,          \
                                    linear_address);                           \
        }                                                                      \
        if (!entry) {                                                          \
            raise_exception(hartptr, EXCEPTION_LOAD_PAGE_FAULT);               \
            __not_reach();                                                     \
        }                                                                      \
        return entry->pmr->pmr_read(entry->pa_tag | ((linear_address & ~(entry->page_mask))),\
                                    _size / 8, hartptr, entry->pmr);           \
    }                                                                          \
    return direct_read##_size(hartptr, linear_address);                        \
}



_(8)
_(16)
_(32)
_(64)
#undef _



#define _(_size)                                                               \
__attribute__((always_inline)) static inline void                              \
vmwrite##_size (struct hart * hartptr, uint32_t linear_address,                \
                uint##_size##_t value)                                         \
{                                                                              \
    struct csr_entry * csr =                                                   \
        &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_SATP];           \
    if (hartptr->privilege_level < PRIVILEGE_LEVEL_MACHINE &&                  \
        csr->csr_blob & 0x80000000) {                                          \
        struct tlb_entry * entry = VA_TO_TLB_ENTRY(hartptr->dtlb,              \
                                                   hartptr->dtlb_cap,          \
                                                   linear_address);            \
        if (!entry) {                                                          \
            walk_page_table(hartptr, linear_address, hartptr->dtlb,            \
                            hartptr->dtlb_cap);                                \
            entry = VA_TO_TLB_ENTRY(hartptr->dtlb, hartptr->dtlb_cap,          \
                                    linear_address);                           \
        }                                                                      \
        if (!entry) {                                                          \
            raise_exception(hartptr, EXCEPTION_STORE_PAGE_FAULT);              \
            __not_reach();                                                     \
        }                                                                      \
        return entry->pmr->pmr_write(entry->pa_tag | ((linear_address & ~(entry->page_mask))),\
                                    _size / 8, value, hartptr, entry->pmr);    \
    }                                                                          \
    return direct_write##_size(hartptr, linear_address, value);                \
}


_(8)
_(16)
_(32)
_(64)
#undef _

int
pa_to_va(struct hart * hartptr, uint32_t pa, struct tlb_entry * tlb,
         int tlb_cap, uint32_t * va);

uint8_t
mmu_read8(struct hart * hartptr, uint32_t location);

uint16_t
mmu_read16(struct hart * hartptr, uint32_t location);

uint32_t
mmu_read32(struct hart * hartptr, uint32_t location);

uint32_t
mmu_read32_aligned(struct hart * hartptr, uint32_t location);

uint32_t
mmu_instruction_read32(struct hart * hartptr, uint32_t instruction_va);


void
mmu_write8(struct hart * hartptr, uint32_t location, uint8_t value);

void
mmu_write16(struct hart * hartptr, uint32_t location, uint16_t value);

void
mmu_write32(struct hart * hartptr, uint32_t location, uint32_t value);

void
mmu_write32_aligned(struct hart * hartptr, uint32_t location, uint32_t value);

#endif
