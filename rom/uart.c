/*
 * Copyright (c) 2020 Jie Zheng
 */

#include <uart.h>


volatile uint8_t* uart16550;
static uint32_t uart16550_reg_shift;
//static uint32_t uart16550_clock = 1843200;

void
uart16550_init(void)
{
    uart16550 = (uint8_t *)0x10000000;
    uart16550_reg_shift = 0;

    uart16550[UART_REG_IER << uart16550_reg_shift] = 0x00;                // Disable all interrupts
    uart16550[UART_REG_LCR << uart16550_reg_shift] = 0x80;                // Enable DLAB (set baud rate divisor)
    uart16550[UART_REG_DLL << uart16550_reg_shift] = 0x03;                // Set divisor (lo byte)
    uart16550[UART_REG_DLM << uart16550_reg_shift] = 0x00;                //             (hi byte)
    uart16550[UART_REG_LCR << uart16550_reg_shift] = 0x03;                // 8 bits, no parity, one stop bit
    uart16550[UART_REG_FCR << uart16550_reg_shift] = 0xC7;                // Enable FIFO, clear them, with 14-byte threshold
}

void
uart16550_putchar(uint8_t ch)
{
  while ((uart16550[UART_REG_LSR << uart16550_reg_shift] & UART_REG_STATUS_TX) == 0);
  uart16550[UART_REG_QUEUE << uart16550_reg_shift] = ch;
}

int
uart16550_getchar()
{
  if (uart16550[UART_REG_LSR << uart16550_reg_shift] & UART_REG_STATUS_RX)
    return uart16550[UART_REG_QUEUE << uart16550_reg_shift];
  return -1;
}
