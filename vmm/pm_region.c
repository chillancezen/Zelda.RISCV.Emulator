/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <pm_region.h>

// XXX: Define MMIO operations globally on a per-vm basis. for simpicity purpose
// I don't put it in a VM's blob.
static struct pm_region_operation  pmr_ops[MAX_NR_PM_REGIONS];
static int nr_pmr_ops = 0;

int
pm_region_operation_compare(const struct pm_region_operation * pmr1,
                            const struct pm_region_operation * pmr2)
{
    ASSERT(pmr1->addr_low <= pmr1->addr_high);
    ASSERT(pmr2->addr_low <= pmr2->addr_high);
    if (pmr1->addr_high <= pmr2->addr_low) {
        return -1;   
    }
    if (pmr1->addr_low >= pmr2->addr_high) {
        return 1;
    }

#if BUILD_TYPE == BUILD_TYPE_DEBUG
    if ((pmr1->addr_low <= pmr2->addr_low &&
         pmr1->addr_high >= pmr2->addr_high) ||
        (pmr2->addr_low <= pmr1->addr_low &&
         pmr2->addr_high >= pmr1->addr_high)) {
        return 0;
    }
    __not_reach();
    return 0;
#else
    return 0;
#endif
}


struct pm_region_operation *
search_pm_region_callback(uint64_t guest_pa)
{
    struct pm_region_operation target = {
        .addr_low = guest_pa,
        .addr_high = guest_pa + 1
    };
    return SEARCH(struct pm_region_operation, pmr_ops, nr_pmr_ops,
                  pm_region_operation_compare, &target) ;
}

void
register_pm_region_operation(const struct pm_region_operation * pmr)
{
    ASSERT(pmr->pmr_read && pmr->pmr_write);
    ASSERT(!search_pm_region_callback(pmr->addr_low))
    ASSERT(nr_pmr_ops < MAX_NR_PM_REGIONS);
    memcpy(&pmr_ops[nr_pmr_ops], pmr, sizeof(struct pm_region_operation));
    nr_pmr_ops += 1;
    SORT(struct pm_region_operation, pmr_ops, nr_pmr_ops, pm_region_operation_compare);
    {
        ASSERT(search_pm_region_callback(pmr->addr_low));
    }
}
