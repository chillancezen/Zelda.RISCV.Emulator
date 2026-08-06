/*
 * Copyright (c) 2020 Jie Zheng
 *
 *      The test finisher, matching the sifive,test0 device that the riscv
 *      virt platform carries. It is how a guest asks to be switched off:
 *      without it "poweroff" in the guest can only spin forever.
 */
#include <vm.h>
#include <stdlib.h>
#include <log.h>
#include <pm_region.h>
#include <syscon.h>
#include <fdt.h>

#define SYSCON_CMD_MASK     0x0000ffff
#define SYSCON_ARG_SHIFT    16

#define SYSCON_FAIL         0x3333
#define SYSCON_PASS         0x5555
#define SYSCON_RESET        0x7777

static uint32_t syscon_base;
static uint32_t syscon_size;

static uint64_t
syscon_read(uint64_t addr, int access_size, struct hart * hartptr,
            struct pm_region_operation * pmr)
{
    return 0;
}

static void
syscon_write(uint64_t addr, int access_size, uint64_t value,
             struct hart * hartptr, struct pm_region_operation * pmr)
{
    uint32_t command = value & SYSCON_CMD_MASK;
    uint32_t argument = value >> SYSCON_ARG_SHIFT;

    switch (command)
    {
        case SYSCON_PASS:
            log_info("\r\nzelda: guest powered off\r\n");
            exit(0);
            break;
        case SYSCON_FAIL:
            log_error("\r\nzelda: guest reported failure, status %d\r\n",
                      argument);
            exit(argument ? argument : 1);
            break;
        case SYSCON_RESET:
            // A real reset would restart the harts; there is nothing sensible
            // to restart into once the kernel has overwritten its own image.
            log_info("\r\nzelda: guest requested reset, stopping instead\r\n");
            exit(0);
            break;
        default:
            log_warn("syscon: unknown command 0x%x\n", command);
            break;
    }
}

void
build_syscon_fdt_node(struct fdt_build_blob * blob)
{
    char name[64];
    sprintf(name, "test@%x", syscon_base);
    fdt_begin_node(blob, name);
    fdt_prop(blob, "compatible", strlen("sifive,test0") + 1, "sifive,test0");
    uint32_t regs[4] = {
        0x0, BIG_ENDIAN32(syscon_base),
        0x0, BIG_ENDIAN32(syscon_size)
    };
    fdt_prop(blob, "reg", 16, regs);
    fdt_end_node(blob);
}

void
syscon_init(struct virtual_machine * vm)
{
    syscon_base = SYSCON_BASE_DEFAULT;
    syscon_size = 0x1000;

    struct pm_region_operation syscon_pmr = {
        .addr_low = syscon_base,
        .addr_high = syscon_base + syscon_size,
        .pmr_read = syscon_read,
        .pmr_write = syscon_write,
        .pmr_desc = "syscon.mmio"
    };
    register_pm_region_operation(&syscon_pmr);
}
