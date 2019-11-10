/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <hart.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void
hart_init(struct hart * hart_instance, int hart_id)
{
    memset(hart_instance, 0x0, sizeof(struct hart));
    hart_instance->hart_id = hart_id;
    hart_instance->translation_cache_size = DEFAULT_TRANSLATION_CACHE_SIZE;
    hart_instance->translation_cache = 
        aligned_alloc(4096, hart_instance->translation_cache_size);
    assert(hart_instance->translation_cache);
}

