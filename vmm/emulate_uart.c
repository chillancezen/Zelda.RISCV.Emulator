/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <pm_region.h>
#include <log.h>
#include <fdt.h>

#define UART_16550_BASE 0x10000000


#define UART_REG_QUEUE     0    // rx/tx fifo data
#define UART_REG_DLL       0    // divisor latch (LSB)
#define UART_REG_IER       1    // interrupt enable register
#define UART_REG_DLM       1    // divisor latch (MSB)
#define UART_REG_FCR       2    // fifo control register
#define UART_REG_LCR       3    // line control register
#define UART_REG_MCR       4    // modem control register
#define UART_REG_LSR       5    // line status register
#define UART_REG_MSR       6    // modem status register
#define UART_REG_SCR       7    // scratch register
#define UART_REG_STATUS_RX 0x01
#define UART_REG_STATUS_TX 0x20

#define UART_LSR_TEMT       0x40
#define UART_LSR_THRE       0x20
#define UART_BUFFER_SIZE 1024

static uint8_t uart_buffer[UART_BUFFER_SIZE + 1];
static int32_t uart_buffer_ptr = 0;

static uint64_t
uart_mmio_read(uint64_t addr, int access_size,
               struct hart * hartptr,
               struct pm_region_operation * pmr)
{
    uint8_t ret = 0;
    ASSERT(access_size == 1);
    switch (addr - UART_16550_BASE)
    {
        case UART_REG_LSR:
            // always ready to transmit
            ret = UART_LSR_TEMT | UART_LSR_THRE;
            break;
        default:
            //__not_reach();    
            break;
    }
    return ret;
}

static void
uart_mmio_write(uint64_t addr, int access_size, uint64_t value,
                struct hart * hartptr, struct pm_region_operation * pmr)
{
    if (access_size != 1) {
        return;
    }
    ASSERT(access_size == 1);
    switch (addr - UART_16550_BASE)
    {
        case UART_REG_FCR:
        case UART_REG_LCR:
            uart_buffer_ptr = 0;
            break;
        case UART_REG_QUEUE:
            uart_buffer[uart_buffer_ptr++] = (uint8_t)value;
            if (uart_buffer_ptr == UART_BUFFER_SIZE || '\n' == (uint8_t)value) {
                uart_buffer[uart_buffer_ptr] = '\x0';
                log_uart("%s", uart_buffer);
                uart_buffer_ptr = 0x0;
            }
            break;
        default:
            //__not_reach();
            break;
    }
}

void
build_uart_fdt_node(struct fdt_build_blob * blob)
{
    char node_name[64];
    sprintf(node_name, "uart@%x", UART_16550_BASE);
    fdt_begin_node(blob, node_name);
    uint32_t clock_frequency = BIG_ENDIAN32(0x00384000);
    uint32_t interrupts = BIG_ENDIAN32(0x0000000a);
    uint32_t interrupts_parent = BIG_ENDIAN32(0x0000000d);
    fdt_prop(blob, "interrupts", 4, &interrupts);
    fdt_prop(blob, "interrupt-parent", 4, &interrupts_parent);
    fdt_prop(blob, "clock-frequency", 4, &clock_frequency);
    fdt_prop(blob, "compatible", strlen("ns16550a") + 1, "ns16550a");
    uint32_t regs[4] = {BIG_ENDIAN32(0), BIG_ENDIAN32(UART_16550_BASE),
                        BIG_ENDIAN32(0), BIG_ENDIAN32(0x100)};
    fdt_prop(blob, "reg", 16, regs);
    fdt_end_node(blob);
}

void
uart_init(void)
{
    struct pm_region_operation uart_mmio_region = {
        .addr_low = UART_16550_BASE,
        .addr_high = UART_16550_BASE + 0x100,
        .pmr_read = uart_mmio_read,
        .pmr_write = uart_mmio_write,
        .pmr_desc = "uart.mmio"
    };
    register_pm_region_operation(&uart_mmio_region);
}
