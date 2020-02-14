/*
 * Copyright (c) 2019 Jie Zheng
 */
#ifndef _MMU_H
#define _MMU_H
#include <vm.h>
#include <hart.h>
#include <pm_region.h>

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



#define _(_size)                                                               \
__attribute__((always_inline)) static inline uint##_size##_t                   \
vmread##_size (struct hart * hartptr, uint32_t linear_address)                 \
{                                                                              \
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
    return direct_write##_size(hartptr, linear_address, value);                \
}


_(8)
_(16)
_(32)
_(64)
#undef _

uint8_t
mmu_read8(struct hart * hartptr, uint32_t location);

uint16_t
mmu_read16(struct hart * hartptr, uint32_t location);

uint32_t
mmu_read32(struct hart * hartptr, uint32_t location);

uint32_t
mmu_read32_aligned(struct hart * hartptr, uint32_t location);



void
mmu_write8(struct hart * hartptr, uint32_t location, uint8_t value);

void
mmu_write16(struct hart * hartptr, uint32_t location, uint16_t value);

void
mmu_write32(struct hart * hartptr, uint32_t location, uint32_t value);

void
mmu_write32_aligned(struct hart * hartptr, uint32_t location, uint32_t value);

#endif
