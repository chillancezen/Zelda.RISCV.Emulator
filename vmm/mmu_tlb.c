/*
 * Copyright (c) 2020 Jie Zheng
 */
#include <mmu_tlb.h>

void
invalidate_tlb(struct tlb_entry * tlb_base, int tlb_cap)
{
    int idx = 0;
    struct tlb_entry * entry;
    for (; idx < tlb_cap; idx++) {
        entry = tlb_base + idx;
        entry->entry_valid = 0;
    }
}
