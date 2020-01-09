/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <vm.h>
#include <string.h>
#include <physical_memory.h>
#include <stdlib.h>
#include <util.h>
#include <pm_region.h>
#include <fcntl.h>
#include <unistd.h>

static uint64_t
main_memory_direct_read(uint64_t addr, int access_size,
                        struct hart * hartptr,
                        struct pm_region_operation * pmr)
{
    uint64_t val = 0;
    struct virtual_machine * vm = hartptr->vmptr;
    void * memory_access_base = vm->main_mem_host_base + (addr - pmr->addr_low);
    switch (access_size)
    {
#define _(size, type)                                                          \
        case size:                                                             \
            val = *(type *)memory_access_base;                                 \
            break
        _(1, uint8_t);
        _(2, uint16_t);
        _(4, uint32_t);
        _(8, uint64_t);
        default:
            __not_reach();
            break;
#undef _
    }
    return val;
}

static void
main_memory_direct_write(uint64_t addr, int access_size, uint64_t value,
                        struct hart * hartptr,
                        struct pm_region_operation * pmr)
{
    struct virtual_machine * vm = hartptr->vmptr;
    void * memory_access_base = vm->main_mem_host_base + (addr - pmr->addr_low);
    switch (access_size)
    {
#define _(size, type)                                                          \
        case size:                                                             \
            *(type *)memory_access_base = (type)value;                         \
            break
        _(1, uint8_t);
        _(2, uint16_t);
        _(4, uint32_t);
        _(8, uint64_t);
        default:
            __not_reach();
            break;
#undef _
    }
}

static void
memory_init(struct virtual_machine * vm)
{
    // after the main memory is allocated. register it as the physical memory region
    // in the global list. it's backed as main memory.
    struct pm_region_operation main_memory_pmr = {
        .addr_low = vm->main_mem_base,
        .addr_high = vm->main_mem_base + vm->main_mem_size,
        .pmr_read = main_memory_direct_read,
        .pmr_write = main_memory_direct_write,
        .pmr_desc = "main.memory"
    };
    register_pm_region_operation(&main_memory_pmr);
}

static void
device_init(struct virtual_machine * vm)
{
    uart_init();
}

static int
preload_binary_image(void * addr, int64_t length, const char * image_path)
{
    int fd = open(image_path, O_RDONLY);
    if (fd < 0) {
        return fd;
    }
    int nr_image_length = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    int nr_left = nr_image_length;
    int nr_read = 0;
    while (nr_left > 0) {
        int nr_to_read = MIN(nr_left, length - nr_read);
        if (!nr_to_read) {
            break;
        }
        int tmp = read(fd, addr + nr_read, nr_to_read);
        if (tmp <= 0) {
            break;
        }
        nr_left -= tmp;
        nr_read += tmp;
    }
    close(fd);
    return !(nr_left == 0);
}

void
virtual_machine_init(struct virtual_machine * vm,
                     const struct virtual_machine_spec * spec)
{
    int idx = 0;
    struct hart * hart_ptr;
    memset(vm, 0x0, sizeof(struct virtual_machine));
    vm->main_mem_base = spec->main_mem_base;
    vm->main_mem_size = MEGA(spec->main_mem_size_in_mega);
    vm->main_mem_host_base = preallocate_physical_memory(vm->main_mem_size);
    ASSERT(vm->main_mem_host_base);
    
    vm->nr_harts = spec->nr_harts;
    vm->boot_hart = spec->boot_cpu;
    vm->harts = aligned_alloc(64, vm->nr_harts * sizeof(struct hart));
    ASSERT(vm->harts);

    for (idx = 0; idx < vm->nr_harts; idx++) {
        hart_ptr = hart_by_id(vm, idx);
        hart_init(hart_ptr, idx);
        hart_ptr->pc = spec->entry_point;
        hart_ptr->vmptr = vm;
    }

    ASSERT(!preload_binary_image(vm->main_mem_host_base + spec->image_load_base - vm->main_mem_base,
                                 vm->main_mem_size, spec->image_path));
    memory_init(vm);
    device_init(vm);

    dump_memory_regions();
}

