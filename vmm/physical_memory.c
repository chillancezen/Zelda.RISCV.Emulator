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
    /*
     * Anonymous mmap hands back zeroed pages and only commits them as the
     * guest touches them, so a large main memory does not cost anything up
     * front. Zeroing matters: the firmware's bss and anything the guest
     * assumes starts clear live in here.
     */
    void * base = mmap(NULL, nr_bytes, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    return base == MAP_FAILED ? NULL : base;
}

