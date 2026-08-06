/*
 * Copyright (c) 2020 Jie Zheng
 */
#include <vm.h>
#include <stdlib.h>
#include <log.h>
#include <pm_region.h>
#include <plic.h>
#include <fdt.h>

#define PLIC_MAX_CONTEXTS (MAX_NR_HARTS * PLIC_CONTEXTS_PER_HART)

// Register map of the SiFive PLIC that the riscv,plic0 binding describes.
#define PLIC_REG_PRIORITY_BASE  0x000000
#define PLIC_REG_PENDING_BASE   0x001000
#define PLIC_REG_ENABLE_BASE    0x002000
#define PLIC_REG_ENABLE_STRIDE  0x80
#define PLIC_REG_CONTEXT_BASE   0x200000
#define PLIC_REG_CONTEXT_STRIDE 0x1000
#define PLIC_REG_CONTEXT_THRESHOLD 0x0
#define PLIC_REG_CONTEXT_CLAIM     0x4

struct plic_state {
    uint32_t base;
    uint32_t size;
    int nr_contexts;
    int phandle;

    uint32_t priority[PLIC_NR_SOURCES];
    // Level state as driven by the devices, and the subset not yet claimed.
    uint32_t asserted;
    uint32_t pending;
    uint32_t claimed;

    uint32_t enable[PLIC_MAX_CONTEXTS];
    uint32_t threshold[PLIC_MAX_CONTEXTS];
};

static struct plic_state plic;

void
plic_set_pending(int source, int asserted)
{
    ASSERT(source > 0 && source < PLIC_NR_SOURCES);
    if (asserted) {
        plic.asserted |= 1u << source;
        plic.pending |= 1u << source;
    } else {
        plic.asserted &= ~(1u << source);
        plic.pending &= ~(1u << source);
    }
}

/*
 * The highest priority interrupt this context is allowed to see, or 0 when
 * none qualifies.
 */
static int
plic_best_source(int context)
{
    int best = 0;
    uint32_t best_priority = 0;
    uint32_t candidates = plic.pending & ~plic.claimed & plic.enable[context];
    int source = 1;

    for (; source < PLIC_NR_SOURCES; source++) {
        if (!(candidates & (1u << source))) {
            continue;
        }
        if (plic.priority[source] <= plic.threshold[context]) {
            continue;
        }
        if (plic.priority[source] > best_priority) {
            best_priority = plic.priority[source];
            best = source;
        }
    }
    return best;
}

void
plic_refresh_pending(struct hart * hartptr)
{
    int mcontext = PLIC_CONTEXT_MACHINE(hartptr->hart_id);
    int scontext = PLIC_CONTEXT_SUPERVISOR(hartptr->hart_id);

    hartptr->ipending.bits.mei = plic_best_source(mcontext) ? 1 : 0;
    hartptr->ipending.bits.sei = plic_best_source(scontext) ? 1 : 0;
}

static uint32_t
plic_claim(int context)
{
    int source = plic_best_source(context);
    if (source) {
        // A claimed source stops contending until the handler completes it.
        plic.claimed |= 1u << source;
    }
    return source;
}

static void
plic_complete(int context, uint32_t source)
{
    if (source == 0 || source >= PLIC_NR_SOURCES) {
        return;
    }
    plic.claimed &= ~(1u << source);
    // Level triggered: if the device is still asserting, it fires again.
    if (plic.asserted & (1u << source)) {
        plic.pending |= 1u << source;
    } else {
        plic.pending &= ~(1u << source);
    }
}

static uint64_t
plic_mmio_read(uint64_t addr, int access_size, struct hart * hartptr,
               struct pm_region_operation * pmr)
{
    uint32_t offset = addr - plic.base;
    uint32_t val = 0;

    if (offset < PLIC_REG_PRIORITY_BASE + 4 * PLIC_NR_SOURCES) {
        val = plic.priority[offset / 4];
    } else if (offset >= PLIC_REG_PENDING_BASE && offset < PLIC_REG_ENABLE_BASE) {
        val = (offset == PLIC_REG_PENDING_BASE) ? plic.pending : 0;
    } else if (offset >= PLIC_REG_ENABLE_BASE && offset < PLIC_REG_CONTEXT_BASE) {
        uint32_t context = (offset - PLIC_REG_ENABLE_BASE) / PLIC_REG_ENABLE_STRIDE;
        uint32_t word = ((offset - PLIC_REG_ENABLE_BASE) % PLIC_REG_ENABLE_STRIDE) / 4;
        val = (context < (uint32_t)plic.nr_contexts && word == 0) ?
              plic.enable[context] : 0;
    } else if (offset >= PLIC_REG_CONTEXT_BASE) {
        uint32_t context = (offset - PLIC_REG_CONTEXT_BASE) / PLIC_REG_CONTEXT_STRIDE;
        uint32_t reg = (offset - PLIC_REG_CONTEXT_BASE) % PLIC_REG_CONTEXT_STRIDE;
        if (context < (uint32_t)plic.nr_contexts) {
            if (reg == PLIC_REG_CONTEXT_THRESHOLD) {
                val = plic.threshold[context];
            } else if (reg == PLIC_REG_CONTEXT_CLAIM) {
                val = plic_claim(context);
            }
        }
    }
    return val;
}

static void
plic_mmio_write(uint64_t addr, int access_size, uint64_t value,
                struct hart * hartptr, struct pm_region_operation * pmr)
{
    uint32_t offset = addr - plic.base;

    if (offset < PLIC_REG_PRIORITY_BASE + 4 * PLIC_NR_SOURCES) {
        // Source 0 has no priority register: it always reads as zero.
        if (offset / 4) {
            plic.priority[offset / 4] = value;
        }
    } else if (offset >= PLIC_REG_PENDING_BASE && offset < PLIC_REG_ENABLE_BASE) {
        // Pending bits are set by the devices, not by software.
    } else if (offset >= PLIC_REG_ENABLE_BASE && offset < PLIC_REG_CONTEXT_BASE) {
        uint32_t context = (offset - PLIC_REG_ENABLE_BASE) / PLIC_REG_ENABLE_STRIDE;
        uint32_t word = ((offset - PLIC_REG_ENABLE_BASE) % PLIC_REG_ENABLE_STRIDE) / 4;
        if (context < (uint32_t)plic.nr_contexts && word == 0) {
            plic.enable[context] = value;
        }
    } else if (offset >= PLIC_REG_CONTEXT_BASE) {
        uint32_t context = (offset - PLIC_REG_CONTEXT_BASE) / PLIC_REG_CONTEXT_STRIDE;
        uint32_t reg = (offset - PLIC_REG_CONTEXT_BASE) % PLIC_REG_CONTEXT_STRIDE;
        if (context < (uint32_t)plic.nr_contexts) {
            if (reg == PLIC_REG_CONTEXT_THRESHOLD) {
                plic.threshold[context] = value;
            } else if (reg == PLIC_REG_CONTEXT_CLAIM) {
                plic_complete(context, value);
            }
        }
    }
}

void
build_plic_fdt_node(struct fdt_build_blob * blob)
{
    struct virtual_machine * vm = CONTAINER_OF(blob, struct virtual_machine, fdt);
    char name[64];
    int idx;

    sprintf(name, "plic@%x", plic.base);
    fdt_begin_node(blob, name);

    uint32_t phandle = BIG_ENDIAN32(plic.phandle);
    fdt_prop(blob, "phandle", 4, &phandle);
    fdt_prop(blob, "compatible", strlen("riscv,plic0") + 1, "riscv,plic0");
    fdt_prop(blob, "interrupt-controller", 0, NULL);
    uint32_t interrupt_cells = BIG_ENDIAN32(1);
    fdt_prop(blob, "#interrupt-cells", 4, &interrupt_cells);
    uint32_t address_cells = BIG_ENDIAN32(0);
    fdt_prop(blob, "#address-cells", 4, &address_cells);
    uint32_t nr_sources = BIG_ENDIAN32(PLIC_NR_SOURCES - 1);
    fdt_prop(blob, "riscv,ndev", 4, &nr_sources);

    uint32_t regs[4] = {
        0x0, BIG_ENDIAN32(plic.base),
        0x0, BIG_ENDIAN32(plic.size)
    };
    fdt_prop(blob, "reg", 16, regs);

    // Two entries per hart: the machine and the supervisor external interrupt
    // of that hart's local controller, in context order.
    uint32_t interrupt_extended[4 * MAX_NR_HARTS];
    for (idx = 0; idx < vm->nr_harts; idx++) {
        uint32_t hart_phandle =
            BIG_ENDIAN32(blob->hart_interrupt_controllers_phandles[idx]);
        interrupt_extended[idx * 4 + 0] = hart_phandle;
        interrupt_extended[idx * 4 + 1] = BIG_ENDIAN32(0xb);
        interrupt_extended[idx * 4 + 2] = hart_phandle;
        interrupt_extended[idx * 4 + 3] = BIG_ENDIAN32(0x9);
    }
    fdt_prop(blob, "interrupts-extended", 16 * vm->nr_harts, interrupt_extended);
    fdt_end_node(blob);
}

int
plic_phandle(void)
{
    return plic.phandle;
}

void
plic_reserve_phandle(struct fdt_build_blob * blob)
{
    plic.phandle = generate_phandle(blob);
}

void
plic_init(struct virtual_machine * vm)
{
    const char * base_string = ini_get(vm->ini_config, "cpu", "plic_base");
    const char * size_string = ini_get(vm->ini_config, "cpu", "plic_size");

    memset(&plic, 0x0, sizeof(plic));
    // Defaults match the addresses the riscv virt platform uses.
    plic.base = base_string ? strtol(base_string, NULL, 16) : 0x0c000000;
    plic.size = size_string ? strtol(size_string, NULL, 16) : 0x04000000;
    plic.nr_contexts = vm->nr_harts * PLIC_CONTEXTS_PER_HART;
    ASSERT(plic.nr_contexts <= PLIC_MAX_CONTEXTS);

    struct pm_region_operation plic_pmr = {
        .addr_low = plic.base,
        .addr_high = plic.base + plic.size,
        .pmr_read = plic_mmio_read,
        .pmr_write = plic_mmio_write,
        .pmr_desc = "plic.mmio"
    };
    register_pm_region_operation(&plic_pmr);
}
