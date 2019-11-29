/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <hart.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
void
hart_init(struct hart * hart_instance, int hart_id)
{
    memset(hart_instance, 0x0, sizeof(struct hart));
    hart_instance->hart_id = hart_id;

    // mprotect requires the memory is obtained by mmap, or its behavior is
    // undefined.
    uint64_t tc_base = (uint64_t)mmap(NULL, TRANSLATION_CACHE_SIZE + 4096,
                                      PROT_READ | PROT_WRITE | PROT_EXEC,
                                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    tc_base &= ~4095;
    hart_instance->translation_cache = (void *)tc_base;
    assert(hart_instance->translation_cache);

    hart_instance->pc_mappings =
        aligned_alloc(4096, MAX_INSTRUCTIONS_TOTRANSLATE *
                            sizeof(struct program_counter_mapping_item));
    assert(hart_instance->pc_mappings);
    flush_translation_cache(hart_instance);

    // grant exec privilege to the translation cache
    assert(!mprotect(hart_instance->translation_cache, TRANSLATION_CACHE_SIZE,
                     PROT_EXEC | PROT_READ | PROT_WRITE));
}


void
flush_translation_cache(struct hart * hart_instance)
{
    hart_instance->nr_translated_instructions = 0;
    hart_instance->translation_cache_ptr = 0;
    printf("*****FLUSH TRANSLATION CACHE*****\n");
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

void
dump_hart(struct hart * hartptr)
{
    const char * regs_abi_names[] = {
        "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };
    printf("dump hart:%p\n", hartptr);
    printf("\thart-id: %d\n", hartptr->hart_id);
    printf("\tpc: 0x%x\n", hartptr->pc);
    int index = 0;
    uint32_t * regs = (uint32_t *)&hartptr->registers;
    for (index = 0; index < 32; index++) {
        printf("\t");
        printf("X%02d(%-4s): 0x%08x  ", index, regs_abi_names[index],
               regs[index]);
        if (((index + 1) % 4) == 0) {
            printf("\n");
        }
    }
}
