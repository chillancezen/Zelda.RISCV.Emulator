/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <physical_memory.h>

#include <stdlib.h>
#include <sys/mman.h>

#define VMM_BASE_PAGE_SIZE 4096

void *
preallocate_physical_memory(int64_t nr_bytes)
{
    return aligned_alloc(VMM_BASE_PAGE_SIZE, nr_bytes);
}

