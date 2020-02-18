/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <mmu.h>
#include <mmu_tlb.h>
#include <hart_exception.h>
#include <csr.h>

uint8_t
mmu_read8(struct hart * hartptr, uint32_t location)
{
    return vmread8(hartptr, location);
}

uint16_t
mmu_read16(struct hart * hartptr, uint32_t location)
{
    return vmread16(hartptr, location);
}


uint32_t
mmu_read32(struct hart * hartptr, uint32_t location)
{
    return vmread32(hartptr, location);
}

uint32_t
mmu_read32_aligned(struct hart * hartptr, uint32_t location)
{
    if (location & 0x3) {
        raise_exception(hartptr, EXCEPTION_LOAD_ADDRESS_MISALIGN);
    }
    return vmread32(hartptr, location);
}



void
mmu_write8(struct hart * hartptr, uint32_t location, uint8_t value)
{
    vmwrite8(hartptr, location, value);
}

void
mmu_write16(struct hart * hartptr, uint32_t location, uint16_t value)
{
    vmwrite16(hartptr, location, value);
}


void
mmu_write32(struct hart * hartptr, uint32_t location, uint32_t value)
{
    vmwrite32(hartptr, location, value);
}


void
mmu_write32_aligned(struct hart * hartptr, uint32_t location, uint32_t value)
{
    if (location & 0x3) {
        raise_exception(hartptr, EXCEPTION_LOAD_ADDRESS_MISALIGN);
    }
    vmwrite32(hartptr, location, value);
}

/*
 * CAVEATS:
 * https://github.com/riscv/riscv-isa-manual/issues/486
 * c0000134:   18061073            csrw    satp,a2
 * c0000138:   12000073            sfence.vma
 * c000013c:   00008067            ret
 *
 */
uint32_t
mmu_instruction_read32(struct hart * hartptr, uint32_t instruction_va)
{
    struct csr_entry * csr = &((struct csr_entry *)hartptr->csrs_base)[CSR_ADDRESS_SATP];
    // MAKE SURE PAGING IS ENABLED IN CURRENT MODE
    if (hartptr->privilege_level <= PRIVILEGE_LEVEL_MACHINE &&
        csr->csr_blob & 0x80000000) {
        struct tlb_entry * entry = VA_TO_TLB_ENTRY(hartptr->itlb,
                                                   hartptr->itlb_cap,
                                                   instruction_va);
        if (!entry) {
            // HIT ITLB MISS, PERFORME A PAGE TABLE WALK AND LOAD THE TLB CACHE
            walk_page_table(hartptr, instruction_va, hartptr->itlb,
                            hartptr->itlb_cap);
            entry = VA_TO_TLB_ENTRY(hartptr->itlb, hartptr->itlb_cap,
                                    instruction_va);
        }
        if (!entry) {
            raise_exception(hartptr, EXCEPTION_INSTRUCTION_PAGE_FAULT);
            __not_reach();
        }
        return entry->pmr->pmr_read(entry->pa_tag | ((instruction_va & ~(entry->page_mask))),
                                    4, hartptr, entry->pmr);
    } else {
        return direct_read32(hartptr, instruction_va);
    }
}
