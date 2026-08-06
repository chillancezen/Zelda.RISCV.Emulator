/*
 * Copyright (c) 2020 Jie Zheng
 */
#include <vm.h>
#include <stdlib.h>
#include <log.h>
#include <pm_region.h>
#include <clint.h>
#include <fdt.h>

struct clint_state {
    uint32_t base;
    uint32_t size;
    int nr_harts;
    uint64_t mtime;
    uint64_t mtimecmp[MAX_NR_HARTS];
    uint32_t msip[MAX_NR_HARTS];
};

static struct clint_state clint;

void
clint_advance_time(uint64_t ticks)
{
    clint.mtime += ticks;
}

uint64_t
clint_get_time(void)
{
    return clint.mtime;
}

int
clint_skip_to_next_deadline(void)
{
    uint64_t earliest = ~0ULL;
    int idx = 0;
    for (; idx < clint.nr_harts; idx++) {
        if (clint.mtimecmp[idx] < earliest) {
            earliest = clint.mtimecmp[idx];
        }
    }
    if (earliest == ~0ULL) {
        return 0;
    }
    if (earliest > clint.mtime) {
        clint.mtime = earliest;
    }
    return 1;
}

void
clint_refresh_pending(struct hart * hartptr)
{
    int id = hartptr->hart_id;
    hartptr->ipending.bits.mti = clint.mtime >= clint.mtimecmp[id] ? 1 : 0;
    hartptr->ipending.bits.msi = clint.msip[id] ? 1 : 0;
}

static uint64_t
clint_read(uint64_t addr, int access_size, struct hart * hartptr,
           struct pm_region_operation * pmr)
{
    uint32_t offset = addr - clint.base;
    uint64_t val = 0;

    if (offset >= CLINT_REG_MSIP && offset < CLINT_REG_MSIP + 4 * MAX_NR_HARTS) {
        uint32_t id = (offset - CLINT_REG_MSIP) / 4;
        val = id < (uint32_t)clint.nr_harts ? clint.msip[id] : 0;
    } else if (offset >= CLINT_REG_MTIMECMP &&
               offset < CLINT_REG_MTIMECMP + 8 * MAX_NR_HARTS) {
        uint32_t id = (offset - CLINT_REG_MTIMECMP) / 8;
        uint64_t cmp = id < (uint32_t)clint.nr_harts ? clint.mtimecmp[id] : 0;
        if (access_size == 8) {
            val = cmp;
        } else {
            val = (offset & 0x4) ? (cmp >> 32) : (cmp & 0xffffffff);
        }
    } else if (offset == CLINT_REG_MTIME) {
        val = access_size == 8 ? clint.mtime : (clint.mtime & 0xffffffff);
    } else if (offset == CLINT_REG_MTIME + 4) {
        val = clint.mtime >> 32;
    } else {
        log_warn("clint: unhandled read at offset 0x%x\n", offset);
    }
    return val;
}

static void
clint_write(uint64_t addr, int access_size, uint64_t value,
            struct hart * hartptr, struct pm_region_operation * pmr)
{
    uint32_t offset = addr - clint.base;

    if (offset >= CLINT_REG_MSIP && offset < CLINT_REG_MSIP + 4 * MAX_NR_HARTS) {
        uint32_t id = (offset - CLINT_REG_MSIP) / 4;
        if (id < (uint32_t)clint.nr_harts) {
            clint.msip[id] = value & 0x1;
        }
    } else if (offset >= CLINT_REG_MTIMECMP &&
               offset < CLINT_REG_MTIMECMP + 8 * MAX_NR_HARTS) {
        uint32_t id = (offset - CLINT_REG_MTIMECMP) / 8;
        if (id < (uint32_t)clint.nr_harts) {
            if (access_size == 8) {
                clint.mtimecmp[id] = value;
            } else if (offset & 0x4) {
                clint.mtimecmp[id] = (clint.mtimecmp[id] & 0xffffffffULL) |
                                     (value << 32);
            } else {
                clint.mtimecmp[id] = (clint.mtimecmp[id] & ~0xffffffffULL) |
                                     (value & 0xffffffff);
            }
        }
    } else if (offset == CLINT_REG_MTIME) {
        clint.mtime = access_size == 8 ?
                      value : ((clint.mtime & ~0xffffffffULL) |
                               (value & 0xffffffff));
    } else if (offset == CLINT_REG_MTIME + 4) {
        clint.mtime = (clint.mtime & 0xffffffffULL) | (value << 32);
    } else {
        log_warn("clint: unhandled write at offset 0x%x\n", offset);
    }
}

void
build_clint_fdt_node(struct fdt_build_blob * blob)
{
    struct virtual_machine * vm = CONTAINER_OF(blob, struct virtual_machine, fdt);
    char name[64];
    int idx;

    sprintf(name, "clint@%x", clint.base);
    fdt_begin_node(blob, name);
    uint32_t regs[4] = {
        0x0, BIG_ENDIAN32(clint.base),
        0x0, BIG_ENDIAN32(clint.size)
    };
    fdt_prop(blob, "reg", 16, regs);
    fdt_prop(blob, "compatible", strlen("riscv,clint0") + 1, "riscv,clint0");

    // Each hart contributes two entries: its machine software interrupt and
    // its machine timer interrupt, both routed to that hart's local
    // interrupt controller.
    uint32_t interrupt_extended[4 * MAX_NR_HARTS];
    for (idx = 0; idx < vm->nr_harts; idx++) {
        uint32_t phandle =
            BIG_ENDIAN32(blob->hart_interrupt_controllers_phandles[idx]);
        interrupt_extended[idx * 4 + 0] = phandle;
        interrupt_extended[idx * 4 + 1] = BIG_ENDIAN32(0x3);
        interrupt_extended[idx * 4 + 2] = phandle;
        interrupt_extended[idx * 4 + 3] = BIG_ENDIAN32(0x7);
    }
    fdt_prop(blob, "interrupts-extended", 16 * vm->nr_harts, interrupt_extended);
    fdt_end_node(blob);
}

void
clint_init(struct virtual_machine * vm)
{
    const char * clint_base_string = ini_get(vm->ini_config, "cpu", "clint_base");
    const char * clint_size_string = ini_get(vm->ini_config, "cpu", "clint_size");
    ASSERT(clint_base_string);
    ASSERT(clint_size_string);

    memset(&clint, 0x0, sizeof(clint));
    clint.base = strtol(clint_base_string, NULL, 16);
    clint.size = strtol(clint_size_string, NULL, 16);
    clint.nr_harts = vm->nr_harts;
    ASSERT(clint.nr_harts <= MAX_NR_HARTS);

    // No timer is armed until the guest programs one.
    int idx = 0;
    for (; idx < MAX_NR_HARTS; idx++) {
        clint.mtimecmp[idx] = ~0ULL;
    }

    struct pm_region_operation clint_pmr = {
        .addr_low = clint.base,
        .addr_high = clint.base + clint.size,
        .pmr_read = clint_read,
        .pmr_write = clint_write,
        .pmr_desc = "clint.mmio"
    };
    register_pm_region_operation(&clint_pmr);
}
