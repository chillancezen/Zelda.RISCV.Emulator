/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <mmu.h>
#include <hart_exception.h>

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

