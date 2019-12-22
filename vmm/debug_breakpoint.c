/*
 * Copyright (c) 2019 Jie Zheng
 */

#include <debug.h>
#include <search.h>
#include <sort.h>
#include <util.h>

#define MAX_BREAKPOINTS 256
static uint32_t breakpoints[MAX_BREAKPOINTS];
static int nr_breakpoints = 0;

static int
addr_cmp(uint32_t * paddr0, uint32_t *paddr1)
{
    return (int)(*paddr0 - *paddr1);
}

int
add_breakpoint(uint32_t guest_addr)
{

    if (nr_breakpoints >= MAX_BREAKPOINTS) {
        return -1;
    }
    breakpoints[nr_breakpoints] = guest_addr;
    nr_breakpoints += 1;
    INSERTION_SORT(uint32_t, breakpoints, nr_breakpoints, addr_cmp);
    return 0;
}

int
is_address_breakpoint(uint32_t guest_addr)
{
    uint32_t * pbp = SEARCH(uint32_t, breakpoints, nr_breakpoints, addr_cmp,
                            &guest_addr);
    return !!pbp;
}

void
dump_breakpoints(void)
{
    printf(ANSI_COLOR_MAGENTA"There are %d breakpoints:\n", nr_breakpoints);
    int idx = 0;
    for (; idx < nr_breakpoints; idx++) {
        printf("0x%08x    ", breakpoints[idx]);
        if ((idx + 1) % 8 == 0) {
            printf("\n");
        }
    }
    if (idx % 8 != 0) {
        printf("\n");
    }
    printf(ANSI_COLOR_RESET);
}
