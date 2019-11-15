/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <hart.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

void
hart_init(struct hart * hart_instance, int hart_id)
{
    memset(hart_instance, 0x0, sizeof(struct hart));
    hart_instance->hart_id = hart_id;
    hart_instance->translation_cache = 
        aligned_alloc(4096, TRANSLATION_CACHE_SIZE);
    assert(hart_instance->translation_cache);

    hart_instance->pc_mappings =
        aligned_alloc(4096, MAX_INSTRUCTIONS_TOTRANSLATE *
                            sizeof(struct program_counter_mapping_item));
    assert(hart_instance->pc_mappings);
    flush_translation_cache(hart_instance);
}


void
flush_translation_cache(struct hart * hart_instance)
{
    hart_instance->nr_translated_instructions = 0; 
}

static int
comparing_mapping_item(const void *a, const void * b)
{
    const struct program_counter_mapping_item * item_a = a;
    const struct program_counter_mapping_item * item_b = b;
    return item_a->guest_pc - item_b->guest_pc;
}

// @return zero upon success, otherwise, non-zero is returned
int
add_translation_item(struct hart * hart_instance,
                     uint32_t guest_instruction_address,
                     const void * translation_instruction_block,
                     int instruction_block_length)
{
    if (unoccupied_cache_size(hart_instance) < instruction_block_length) {
        // No enough room for newly translated block, give up.
        return -1;
    }
    if (hart_instance->nr_translated_instructions >= MAX_INSTRUCTIONS_TOTRANSLATE) {
        // No enough room in the mapping array
        return -2; 
    }
    uint32_t tc_offset = hart_instance->translation_cache_ptr;
    memcpy(hart_instance->translation_cache + hart_instance->translation_cache_ptr,
           translation_instruction_block, instruction_block_length);
    hart_instance->translation_cache_ptr += instruction_block_length;

    // insert one mapping entry into per-hart pc mapping array
    struct program_counter_mapping_item * mappings = hart_instance->pc_mappings;
    mappings[hart_instance->nr_translated_instructions].guest_pc =
        guest_instruction_address;
    mappings[hart_instance->nr_translated_instructions].tc_offset = tc_offset;
    hart_instance->nr_translated_instructions ++;
    qsort(hart_instance->pc_mappings, hart_instance->nr_translated_instructions,
          sizeof(struct program_counter_mapping_item), comparing_mapping_item);
    return 0;    
}

struct program_counter_mapping_item *
search_translation_item(struct hart * hart_instance,
                        uint32_t guest_instruction_address)
{
    struct program_counter_mapping_item key = {
        .guest_pc = guest_instruction_address
    };
    return bsearch(&key, hart_instance->pc_mappings,
                   hart_instance->nr_translated_instructions,
                   sizeof(struct program_counter_mapping_item),
                   comparing_mapping_item);
}

