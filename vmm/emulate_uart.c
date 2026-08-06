/*
 * Copyright (c) 2019 Jie Zheng
 */
#include <pm_region.h>
#include <log.h>
#include <fdt.h>
#include <plic.h>
#include <uart.h>
#include <unistd.h>
#include <termios.h>
#include <poll.h>
#include <stdlib.h>
#include <signal.h>

#define UART_16550_BASE 0x10000000

#define UART_REG_QUEUE     0    // rx/tx fifo data
#define UART_REG_DLL       0    // divisor latch (LSB)
#define UART_REG_IER       1    // interrupt enable register
#define UART_REG_DLM       1    // divisor latch (MSB)
#define UART_REG_IIR       2    // interrupt identification register (read)
#define UART_REG_FCR       2    // fifo control register (write)
#define UART_REG_LCR       3    // line control register
#define UART_REG_MCR       4    // modem control register
#define UART_REG_LSR       5    // line status register
#define UART_REG_MSR       6    // modem status register
#define UART_REG_SCR       7    // scratch register

#define UART_IER_RX_AVAIL   0x01
#define UART_IER_THR_EMPTY  0x02

#define UART_IIR_NO_INT     0x01
#define UART_IIR_THR_EMPTY  0x02
#define UART_IIR_RX_AVAIL   0x04
#define UART_IIR_FIFO       0xc0

#define UART_LCR_DLAB       0x80

#define UART_LSR_DR         0x01
#define UART_LSR_THRE       0x20
#define UART_LSR_TEMT       0x40

#define UART_MSR_CTS        0x10
#define UART_MSR_DSR        0x20
#define UART_MSR_DCD        0x80

#define UART_RX_FIFO_SIZE   256

/*
 * The escape sequence that lets the operator get out of a guest which has the
 * terminal in raw mode: ctrl-a followed by x. ctrl-a twice sends a literal
 * ctrl-a through to the guest.
 */
#define UART_ESCAPE_CHAR 0x01

struct uart_state {
    uint8_t ier;
    uint8_t lcr;
    uint8_t mcr;
    uint8_t fcr;
    uint8_t scr;
    uint8_t dll;
    uint8_t dlm;

    uint8_t rx_fifo[UART_RX_FIFO_SIZE];
    int rx_head;
    int rx_tail;

    int console_attached;
    int escape_armed;
};

static struct uart_state uart;
static struct termios saved_termios;
static int termios_saved = 0;

static void
uart_restore_terminal(void)
{
    if (termios_saved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios);
        termios_saved = 0;
    }
}

static void
uart_terminal_signal(int signo)
{
    uart_restore_terminal();
    _exit(128 + signo);
}

/*
 * Put the controlling terminal in raw mode so that guest input behaves like a
 * real serial line: no line buffering, no echo, and control characters
 * delivered to the guest rather than interpreted by the host.
 */
static void
uart_setup_terminal(void)
{
    struct termios raw;

    if (!isatty(STDIN_FILENO)) {
        // Still usable when stdin is a pipe or a file, just not interactive.
        uart.console_attached = 1;
        return;
    }
    if (tcgetattr(STDIN_FILENO, &saved_termios)) {
        return;
    }
    raw = saved_termios;
    raw.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
    raw.c_iflag &= ~(IXON | ICRNL | INLCR | IGNCR | BRKINT | ISTRIP);
    raw.c_oflag &= ~OPOST;
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw)) {
        return;
    }
    termios_saved = 1;
    uart.console_attached = 1;
    atexit(uart_restore_terminal);
    signal(SIGINT, uart_terminal_signal);
    signal(SIGTERM, uart_terminal_signal);
    signal(SIGSEGV, uart_terminal_signal);
    signal(SIGABRT, uart_terminal_signal);
}

static int
uart_rx_empty(void)
{
    return uart.rx_head == uart.rx_tail;
}

static void
uart_rx_push(uint8_t ch)
{
    int next = (uart.rx_tail + 1) % UART_RX_FIFO_SIZE;
    if (next == uart.rx_head) {
        // Fifo full: drop, exactly as a real overrun would.
        return;
    }
    uart.rx_fifo[uart.rx_tail] = ch;
    uart.rx_tail = next;
}

static uint8_t
uart_rx_pop(void)
{
    uint8_t ch;
    if (uart_rx_empty()) {
        return 0;
    }
    ch = uart.rx_fifo[uart.rx_head];
    uart.rx_head = (uart.rx_head + 1) % UART_RX_FIFO_SIZE;
    return ch;
}

/*
 * Drain whatever the host terminal has for us. Called from the device pump at
 * translation unit boundaries, so it must never block.
 */
void
uart_poll_input(void)
{
    struct pollfd pfd = {.fd = STDIN_FILENO, .events = POLLIN};
    uint8_t buf[64];
    int idx;
    int n;

    if (!uart.console_attached) {
        return;
    }
    if (poll(&pfd, 1, 0) <= 0) {
        return;
    }
    n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n <= 0) {
        return;
    }
    for (idx = 0; idx < n; idx++) {
        if (uart.escape_armed) {
            uart.escape_armed = 0;
            if (buf[idx] == 'x') {
                log_info("\r\nzelda: terminating on operator request\r\n");
                uart_restore_terminal();
                exit(0);
            }
            // ctrl-a ctrl-a sends one literal ctrl-a to the guest.
            if (buf[idx] == UART_ESCAPE_CHAR) {
                uart_rx_push(UART_ESCAPE_CHAR);
                continue;
            }
            uart_rx_push(buf[idx]);
            continue;
        }
        if (buf[idx] == UART_ESCAPE_CHAR) {
            uart.escape_armed = 1;
            continue;
        }
        uart_rx_push(buf[idx]);
    }
}

/*
 * The transmit holding register is never actually busy here, so a transmit
 * interrupt is pending whenever the guest asked for one.
 */
static int
uart_interrupt_pending(void)
{
    if ((uart.ier & UART_IER_RX_AVAIL) && !uart_rx_empty()) {
        return 1;
    }
    if (uart.ier & UART_IER_THR_EMPTY) {
        return 1;
    }
    return 0;
}

void
uart_refresh_interrupt(void)
{
    plic_set_pending(PLIC_IRQ_UART0, uart_interrupt_pending());
}

static uint64_t
uart_mmio_read(uint64_t addr, int access_size, struct hart * hartptr,
               struct pm_region_operation * pmr)
{
    uint32_t reg = addr - UART_16550_BASE;
    uint8_t ret = 0;

    switch (reg)
    {
        case UART_REG_QUEUE:
            if (uart.lcr & UART_LCR_DLAB) {
                ret = uart.dll;
            } else {
                ret = uart_rx_pop();
            }
            break;
        case UART_REG_IER:
            ret = (uart.lcr & UART_LCR_DLAB) ? uart.dlm : uart.ier;
            break;
        case UART_REG_IIR:
            if ((uart.ier & UART_IER_RX_AVAIL) && !uart_rx_empty()) {
                ret = UART_IIR_RX_AVAIL;
            } else if (uart.ier & UART_IER_THR_EMPTY) {
                ret = UART_IIR_THR_EMPTY;
            } else {
                ret = UART_IIR_NO_INT;
            }
            // Advertise the 16550A fifo so Linux probes us as a 16550A.
            ret |= UART_IIR_FIFO;
            break;
        case UART_REG_LCR:
            ret = uart.lcr;
            break;
        case UART_REG_MCR:
            ret = uart.mcr;
            break;
        case UART_REG_LSR:
            // Transmission is instantaneous, so the holding register and the
            // shift register always read as empty.
            ret = UART_LSR_TEMT | UART_LSR_THRE;
            if (!uart_rx_empty()) {
                ret |= UART_LSR_DR;
            }
            break;
        case UART_REG_MSR:
            ret = UART_MSR_CTS | UART_MSR_DSR | UART_MSR_DCD;
            break;
        case UART_REG_SCR:
            ret = uart.scr;
            break;
        default:
            break;
    }
    uart_refresh_interrupt();
    return ret;
}

static void
uart_mmio_write(uint64_t addr, int access_size, uint64_t value,
                struct hart * hartptr, struct pm_region_operation * pmr)
{
    uint32_t reg = addr - UART_16550_BASE;
    uint8_t val = (uint8_t)value;

    if (access_size != 1) {
        return;
    }
    switch (reg)
    {
        case UART_REG_QUEUE:
            if (uart.lcr & UART_LCR_DLAB) {
                uart.dll = val;
            } else {
                // Straight through to our own stdout: the guest owns the
                // terminal while it is running.
                ssize_t ignored = write(STDOUT_FILENO, &val, 1);
                (void)ignored;
            }
            break;
        case UART_REG_IER:
            if (uart.lcr & UART_LCR_DLAB) {
                uart.dlm = val;
            } else {
                uart.ier = val & 0x0f;
            }
            break;
        case UART_REG_FCR:
            uart.fcr = val;
            if (val & 0x2) {
                uart.rx_head = uart.rx_tail = 0;
            }
            break;
        case UART_REG_LCR:
            uart.lcr = val;
            break;
        case UART_REG_MCR:
            uart.mcr = val;
            break;
        case UART_REG_SCR:
            uart.scr = val;
            break;
        default:
            break;
    }
    uart_refresh_interrupt();
}

void
build_uart_fdt_node(struct fdt_build_blob * blob)
{
    char node_name[64];
    sprintf(node_name, "uart@%x", UART_16550_BASE);
    fdt_begin_node(blob, node_name);
    uint32_t clock_frequency = BIG_ENDIAN32(0x00384000);
    uint32_t interrupts = BIG_ENDIAN32(PLIC_IRQ_UART0);
    uint32_t interrupt_parent = BIG_ENDIAN32(plic_phandle());
    uint32_t reg_shift = BIG_ENDIAN32(0);
    uint32_t reg_io_width = BIG_ENDIAN32(1);
    fdt_prop(blob, "interrupts", 4, &interrupts);
    fdt_prop(blob, "interrupt-parent", 4, &interrupt_parent);
    fdt_prop(blob, "clock-frequency", 4, &clock_frequency);
    fdt_prop(blob, "reg-shift", 4, &reg_shift);
    fdt_prop(blob, "reg-io-width", 4, &reg_io_width);
    fdt_prop(blob, "compatible", strlen("ns16550a") + 1, "ns16550a");
    uint32_t regs[4] = {BIG_ENDIAN32(0), BIG_ENDIAN32(UART_16550_BASE),
                        BIG_ENDIAN32(0), BIG_ENDIAN32(0x100)};
    fdt_prop(blob, "reg", 16, regs);
    fdt_end_node(blob);
}

void
uart_init(void)
{
    memset(&uart, 0x0, sizeof(uart));
    uart.dll = 0x03;
    uart_setup_terminal();

    struct pm_region_operation uart_mmio_region = {
        .addr_low = UART_16550_BASE,
        .addr_high = UART_16550_BASE + 0x100,
        .pmr_read = uart_mmio_read,
        .pmr_write = uart_mmio_write,
        .pmr_desc = "uart.mmio"
    };
    register_pm_region_operation(&uart_mmio_region);
}
