/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <pm_region.h>

#define UART_RX_BASE 0x10000000
#define UART_TX_BASE 0x10000000


#define UART_BUFFER_SIZE 1024

static uint8_t uart_buffer[UART_BUFFER_SIZE + 1];
static int32_t uart_buffer_ptr = 0;

static uint64_t
uart_mmio_read(uint64_t addr, int access_size,
               struct hart * hartptr,
               struct pm_region_operation * pmr)
{
    ASSERT(addr == UART_RX_BASE);
    return 0;    
}

static void
uart_mmio_write(uint64_t addr, int access_size, uint64_t value,
                struct hart * hartptr, struct pm_region_operation * pmr)
{
    ASSERT(addr == UART_TX_BASE);
    ASSERT(access_size == 1);
    uart_buffer[uart_buffer_ptr++] = (uint8_t)value;
    if (uart_buffer_ptr == UART_BUFFER_SIZE || '\n' == (uint8_t)value) {
        uart_buffer[uart_buffer_ptr] = '\x0';
        printf("%s", uart_buffer);
        uart_buffer_ptr = 0x0;
    }
}
void
uart_init(void)
{
    struct pm_region_operation uart_mmio_region = {
        .addr_low = 0x10000000,
        .addr_high = 0x10000000 + 0x1000,
        .pmr_read = uart_mmio_read,
        .pmr_write = uart_mmio_write,
        .pmr_desc = "uart.mmio"
    };
    register_pm_region_operation(&uart_mmio_region);
}
