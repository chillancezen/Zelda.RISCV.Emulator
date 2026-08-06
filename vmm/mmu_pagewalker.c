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

/*
 * Read one page table entry. Page tables normally live in ram, which can hand
 * back a host pointer, but a guest is free to point satp at anything. Regions
 * that cannot produce a direct pointer, or an address that is not backed at
 * all, mean the table itself is unreadable.
 */
static struct sv32_pte *
pte_at(struct hart * hartptr, uint32_t pa)
{
    struct pm_region_operation * pmr = search_pm_region_callback(pa);
    if (!pmr || !pmr->pmr_direct) {
        return NULL;
    }
    return pmr->pmr_direct(pa, hartptr, pmr);
}

/*
 * Returns 0 on success, or a negative value if the translation could not be
 * completed - in which case no tlb entry is installed and the caller raises a
 * page fault to the guest.
 */
int
walk_page_table(struct hart * hartptr, uint32_t va, struct tlb_entry * tlb,
             int tlb_cap)
{
    struct csr_entry * csr = &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_SATP];
    uint32_t satp = csr->csr_blob;
    ASSERT(satp & 0x80000000);
    /*
     * Truncating the shift drops the mode and asid fields for free, and also
     * the two ppn bits that would address beyond 4GiB - which this vmm does
     * not model anyway.
     */
    uint32_t level1_base = satp << 12;
    uint32_t level1_entry_offset = ((va >> 22) & 0x3ff) * sizeof(struct sv32_pte);
    struct sv32_pte * level1_pte =
        pte_at(hartptr, level1_base + level1_entry_offset);
    if (!level1_pte) {
        log_warn("page walk: root table at 0x%x is not readable memory "
                 "(va 0x%x satp 0x%x)\n", level1_base, va, satp);
        return -3;
    }
    if (!level1_pte->valid) {
        return -1;
    }
    uint32_t level1_ppn = (*(const uint32_t *)level1_pte) >> 10;
    if (level1_pte->read || level1_pte->write || level1_pte->execute) {
        // this is a 4M hugepage
        struct tlb_entry * entry_4m = VA_TO_4M_TLB_ENTRY(tlb, tlb_cap, va);
        uint32_t pa = (level1_ppn << 12) & PAGE_MASK_4M;
        struct pm_region_operation * pmr = search_pm_region_callback(pa);
        if (!pmr) {
            log_warn("page walk: va 0x%x maps to unbacked physical 0x%x "
                     "(4M pte 0x%x, pc 0x%x, satp 0x%x, root 0x%x, idx %d)\n",
                     va, pa, *(const uint32_t *)level1_pte, hartptr->pc, satp,
                     level1_base, (va >> 22) & 0x3ff);
            return -4;
        }
        entry_4m->page_mask = PAGE_MASK_4M;
        entry_4m->va_tag = va & PAGE_MASK_4M;
        entry_4m->pa_tag = pa;
        entry_4m->entry_valid = 1;
        entry_4m->level1_pte = level1_pte;
        entry_4m->level2_pte = NULL;
        entry_4m->pmr = pmr;
    } else {
        // This is a 4K page
        uint32_t level2_base = level1_ppn << 12;
        uint32_t level2_entry_offset = ((va >> 12) & 0x3ff) * sizeof(struct sv32_pte);
        struct sv32_pte * level2_pte =
            pte_at(hartptr, level2_base + level2_entry_offset);
        if (!level2_pte) {
            log_warn("page walk: leaf table at 0x%x is not readable memory "
                     "(va 0x%x)\n", level2_base, va);
            return -3;
        }
        if (!level2_pte->valid) {
            return -2;
        }
        uint32_t level2_ppn = (*(const uint32_t *)level2_pte) >> 10;

        struct tlb_entry * entry_4k = VA_TO_4K_TLB_ENTRY(tlb, tlb_cap, va);
        uint32_t pa = (level2_ppn << 12) & PAGE_MASK_4K;
        struct pm_region_operation * pmr = search_pm_region_callback(pa);
        if (!pmr) {
            log_warn("page walk: va 0x%x maps to unbacked physical 0x%x "
                     "(4K pte 0x%x)\n", va, pa, *(const uint32_t *)level2_pte);
            return -4;
        }
        entry_4k->page_mask = PAGE_MASK_4K;
        entry_4k->va_tag = va & PAGE_MASK_4K;
        entry_4k->pa_tag = pa;
        entry_4k->entry_valid = 1;
        entry_4k->level1_pte = level1_pte;
        entry_4k->level2_pte = level2_pte;
        entry_4k->pmr = pmr;
    }

    return 0;
}



