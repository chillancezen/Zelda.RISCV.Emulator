/*
 * Copyright (c) 2020 Jie Zheng
 */

#ifndef _MMU_TLB_H
#define _MMU_TLB_H
#include <stdint.h>
#include <pm_region.h>

#define PAGE_SHIFT_4K   12
#define PAGE_SHIFT_4M   22

#define PAGE_MASK_4K    0xfffff000
#define PAGE_MASK_4M    0xffc00000

struct sv32_pte {
    uint32_t valid:1;
    uint32_t read:1;
    uint32_t write:1;
    uint32_t execute:1;
    uint32_t user_access:1;
    uint32_t global:1;
    uint32_t accessed:1;
    uint32_t dirty:1;
    uint32_t rsw:2;
    uint32_t ppn0:10;
    uint32_t ppn1:10;
    uint32_t ppn2:2; // we don't physical address space beyond 4GB, so ignore these two bits
}__attribute__((packed));
// WE DESIGN TLB AS ONE WAY SET ASSOCIATIVE ADDRESS TRANSLATION CACHE
// FIXME: WE DON'T SUPPORT ASID, WE MAY SUPPORT IT ONEDAY

struct tlb_entry {
    // PAGE_SHIFT == 12(4K page) or 22()
    uint32_t va_tag; // va << PAGE_SHIFT
    uint32_t pa_tag; // pa << PAGE_SHIFT
    uint32_t page_mask;
    uint32_t entry_valid:1;
    struct sv32_pte * level1_pte;
    struct sv32_pte * level2_pte;
    struct pm_region_operation * pmr;
};


void
invalidate_tlb(struct tlb_entry * tlb_base, int tlb_cap);


#define VA_TO_4K_TLB_ENTRY(tlb_base, tlb_cap, va) ({                           \
    uint32_t __offset = ((uint32_t)(va)) >> PAGE_SHIFT_4K;                     \
    __offset &= ((tlb_cap) - 1);                                               \
    ((struct tlb_entry *)tlb_base) + __offset;                                 \
})


#define VA_TO_4M_TLB_ENTRY(tlb_base, tlb_cap, va) ({                           \
    uint32_t __offset = ((uint32_t)(va)) >> PAGE_SHIFT_4M;                     \
    __offset &= ((tlb_cap) - 1);                                               \
    ((struct tlb_entry *)tlb_base) + __offset;                                 \
})


#define VA_TO_TLB_ENTRY(tlb_base, tlb_cap, va) ({                              \
    struct tlb_entry * __entry = VA_TO_4K_TLB_ENTRY(tlb_base, tlb_cap, va);    \
    if (!__entry->entry_valid) {                                               \
        __entry = VA_TO_4M_TLB_ENTRY(tlb_base, tlb_cap, va);                   \
    }                                                                          \
    __entry->entry_valid ? __entry : NULL;                                     \
})


#endif
