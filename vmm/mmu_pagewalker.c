/*
 * Copyright (c) 2020 Jie Zheng
 */

#include <hart.h>
#include <csr.h>
#include <mmu_tlb.h>
#include <pm_region.h>

#define MAX_PTES 1024

int
pa_to_va(struct hart * hartptr, uint32_t pa, struct tlb_entry * tlb,
         int tlb_cap, uint32_t * va)
{
    int idx = 0;
    struct csr_entry * csr = &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_SATP];
    uint32_t satp = csr->csr_blob;
    ASSERT(satp & 0x80000000);
    uint32_t level1_base = satp << 12;
    struct pm_region_operation * level1_pmr;
    ASSERT((level1_pmr = search_pm_region_callback(level1_base)));
    struct sv32_pte * level1_pte = level1_pmr->pmr_direct(level1_base, hartptr, level1_pmr);
    for (idx = 0; idx < MAX_PTES; idx++) {
        struct sv32_pte * pte = level1_pte + idx;
        if (!pte->valid) {
            continue;
        }
        if (pte->read || pte->write || pte->execute) {
            // 4M paging matching
            uint32_t ppn_4m = ((*(uint32_t *)pte) >> 10) << 12;
            if ((ppn_4m & PAGE_MASK_4M) == (pa & PAGE_MASK_4M)) {
                *va = (pa & ~PAGE_MASK_4M) | (((uint32_t)idx) << 22);
                return 0;
            }
        } else {
            //dump_hart(hartptr);
            __not_reach();
        }
    }
    return -1;
}

int
walk_page_table(struct hart * hartptr, uint32_t va, struct tlb_entry * tlb,
             int tlb_cap)
{
    struct csr_entry * csr = &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_SATP];
    uint32_t satp = csr->csr_blob;
    ASSERT(satp & 0x80000000);
    uint32_t level1_base = satp << 12;
    uint32_t level1_entry_offset = ((va >> 22) & 0x3ff) * sizeof(struct sv32_pte);
    struct pm_region_operation * level1_pmr;
    ASSERT((level1_pmr = search_pm_region_callback(level1_base + level1_entry_offset)));
    struct sv32_pte * level1_pte =
        level1_pmr->pmr_direct(level1_base + level1_entry_offset, hartptr, level1_pmr);
    if (!level1_pte->valid) {
        return -1;
    }
    uint32_t level1_ppn = (*(const uint32_t *)level1_pte) >> 10;
    if (level1_pte->read || level1_pte->write || level1_pte->execute) {
        // this is a 4M hugepage
        struct tlb_entry * entry_4m = VA_TO_4M_TLB_ENTRY(tlb, tlb_cap, va);
        entry_4m->page_mask = PAGE_MASK_4M;
        entry_4m->va_tag = va & PAGE_MASK_4M;
        entry_4m->pa_tag = (level1_ppn << 12) & PAGE_MASK_4M;
        entry_4m->entry_valid = 1;
        entry_4m->level1_pte = level1_pte;
        entry_4m->level2_pte = NULL;
        entry_4m->pmr = search_pm_region_callback(entry_4m->pa_tag);
        ASSERT(entry_4m->pmr);
    } else {
        // This is a 4K page
        uint32_t level2_base = level1_ppn << 12;
        uint32_t level2_entry_offset = ((va >> 12) & 0x3ff) * sizeof(struct sv32_pte);
        struct pm_region_operation * level2_pmr;
        ASSERT((level2_pmr = search_pm_region_callback(level2_base + level2_entry_offset)));
        struct sv32_pte * level2_pte =
            level2_pmr->pmr_direct(level2_base + level2_entry_offset, hartptr, level2_pmr);
        if (!level2_pte->valid) {
            return -2;
        }
        uint32_t level2_ppn = (*(const uint32_t *)level2_pte) >> 10;

        struct tlb_entry * entry_4k = VA_TO_4K_TLB_ENTRY(tlb, tlb_cap, va);
        entry_4k->page_mask = PAGE_MASK_4K;
        entry_4k->va_tag = va & PAGE_MASK_4K;
        entry_4k->pa_tag = (level2_ppn << 12) & PAGE_MASK_4K;
        entry_4k->entry_valid = 1;
        entry_4k->level1_pte = level1_pte;
        entry_4k->level2_pte = level2_pte;
        entry_4k->pmr = search_pm_region_callback(entry_4k->pa_tag);
        ASSERT(entry_4k->pmr);
    }

    return 0;
}



