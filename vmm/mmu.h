/*
 * Copyright (c) 2019 Jie Zheng
 */
#ifndef _MMU_H
#define _MMU_H
#include <vm.h>
#include <hart.h>

#define _(_size)                                                               \
__attribute__((always_inline)) static inline uint##_size##_t                   \
direct_read##_size (struct virtual_machine * vm, uint32_t linear_address)      \
{                                                                              \
    uint##_size##_t ret = 0;                                                   \
    if (linear_address >= 0 && (linear_address + _size / 8) < vm->pmem_size) { \
        ret = *(uint##_size##_t *)(vm->pmem_base + linear_address);            \
    }                                                                          \
    return ret;                                                                \
}



_(8)
_(16)
_(32)
_(64)
#undef _


#define _(_size)                                                               \
__attribute__((always_inline)) static inline void                              \
direct_write##_size (struct virtual_machine * vm, uint32_t linear_address,     \
                     uint##_size##_t value)                                    \
{                                                                              \
    if (linear_address >= 0 && (linear_address + _size / 8) < vm->pmem_size) { \
        *(uint##_size##_t *)(vm->pmem_base + linear_address) = value;          \
    }                                                                          \
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
    struct virtual_machine * vm = hartptr->vmptr;                              \
    return direct_read##_size(vm, linear_address);                             \
}


_(8)
_(16)
_(32)
_(64)
#undef _



#define _(_size)                                                               \
__attribute__((always_inline)) static inline void                              \
vmwrite##_size (struct hart * hartptr, uint32_t linear_address,                \
               uint##_size##_t value)                                          \
{                                                                              \
    struct virtual_machine * vm = hartptr->vmptr;                              \
    return direct_write##_size(vm, linear_address, value);                     \
}


_(8)
_(16)
_(32)
_(64)
#undef _


#endif
