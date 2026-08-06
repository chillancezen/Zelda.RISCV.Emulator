/*
 * Copyright (c) 2020 Jie Zheng
 *
 *      NS16550 console: guest output goes to our stdout, guest input comes
 *      from our stdin.
 */

#ifndef _EMULATE_UART_H
#define _EMULATE_UART_H

struct fdt_build_blob;

/*
 * Non-blocking drain of the host terminal into the receive fifo. Called from
 * the device pump at translation unit boundaries.
 */
void
uart_poll_input(void);

/*
 * Recompute whether the uart is asserting its PLIC line.
 */
void
uart_refresh_interrupt(void);

void
build_uart_fdt_node(struct fdt_build_blob * blob);

void
uart_init(void);

#endif
