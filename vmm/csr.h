/*
 * Copyright (c) 2019 Jie Zheng
 */
#ifndef _CSR_H
#define _CSR_H
#include <stdint.h>
#include <hart.h>
#include <sys/queue.h>
struct csr_entry {
    uint32_t is_valid:1;
    uint32_t csr_blob;

    // on write: csr_blob &= wpri_mask
    // on read: rd &= wpri_mask 
    uint32_t wpri_mask;

    void (*write)(struct hart *hartptr, struct csr_entry * csr, uint32_t value);
    uint32_t (*read)(struct hart *hartptr, struct csr_entry *csr);
    void (*reset)(struct hart *hartptr, struct csr_entry *csr);
};

#define NR_CSRS 4096


struct csr_registery_entry {
    struct csr_registery_entry * next;
    int csr_addr;
    // the fields are legal to register:
    // wpri_mask, write, read, reset
    struct csr_entry csr_registery;
};

extern struct csr_registery_entry * csr_registery_head;


static inline void
register_csr_entry(struct csr_registery_entry * entry)
{
    entry->next = csr_registery_head;
    csr_registery_head = entry;
}


#endif
