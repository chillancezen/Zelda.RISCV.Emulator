# Zelda.RISCV.Emulator
## What's this?
It's a system level emulator which utilizes dynamic binary translation to translate RISCV32 instructions and emulate RISCV32 ISA.
the following diagram shows how the emulator is organized. 
```
        +------------------------------------+
        |---------------------------------------+
        || hart 0                               |
        ||       +------------+--------------+  |       (raising load/store exception)
        ||       | Exception  | Interrupt    |  | <-----------------------------------+
  +----------->  +------------+--------------+  |                                     |
d |     ||       |        TRAP handling      |  |                                     |
e |     ||       +---------------------------+  |                                     |
l |     ||                                      |         +----------------------------------------------+
i |     ||                                      |         |   Memory Management Unit  |                  |
v |     ||    +------+---------------+          |         |                           +    +-----------+ |
e |     ||    | CSRs | Registers| PC |          |         |    +---------+                 |           | |
r |     ||    +------+---------------+          |  +----> |    |  iTLB   | +-------------+ | main mem  | |
  |     ||                                      |  |      |    +---------+      +--------+ |           | |
e |     ||   +--------------------------------+ |  |      |                     |          +-----------+ |
x |     ||   | dynamic binary translation     | |  |      |                     |                        |
t |     ||   | +------------------------------+ |  |      |    +---------+      |          +-----------+ |
e |     ||   | |   translation cache          | |  | +--> |    |  dTLB   | +----+--------+ |           | |
r |     ||   | +------------------------------+ |  | |    |    +---------+   Page Walker   |  io mem   | |
n |     ||   | |   x86_64 translation         | |  | |    |                                |           | |
a |     ||   +-+---+--------+-----------------+ |  | |    |                                +-----------+ |
l |     ++         |        |                   |  | |    |                                              |
  |      +--------------------------------------+  | |    +----------------------------------------------+
i |                |        |                      | |
n |                |        +----------------------+ |
t |                |         (mmu instruction load)  |
e |                |                                 |
r |                +---------------------------------+
r |                       (mmu data load/store)
u |
p |
t |    +-----------------------------------------+
  +----+  Platform-level Interrupt Controller    |
       |                                         |
       +-----------------------------------------+

```

## Try it out
I built a binary executable plus bootloader and Linux image. 
you can download(https://github.com/chillancezen/Zelda.RISCV.Emulator/releases) and decompress it, then run by:
```
#./vmx test.vm.ini
```

## How to build?
Host env: a x86_64 Linux host + gcc 4.8.x

there are lots of things to build in order to run a riscv Linux. here is the guide page: https://risc-v-getting-started-guide.readthedocs.io/en/latest/linux-qemu.html
- riscv32 cross-compile toolchain: `riscv32-unknown-linux-gnu-` 
- riscv-Linux, make sure a newer Linux kernel source is built from, Linux 5.4.0 is chosen in my testbed.
- SBI software: Berkely Bootloader(aka. BBL) is my choice.
- initramfs cpio built with busybox.
- build the emulator by simplily running `CC=gcc-4.8 make`
## How to run ?
Now you should be able to find an executable:`vmx` under directory vmm. that's the virtual machine monitor.
in order to run a guest, we have to define a configuration file to instruct vmm how to load and run a guest. 

A typical config file is given in  **test.vm.ini**
 ```ini

[image]
name = test.guest
; The BBL+Linux path
kernel = /root/workspace/riscv-pk-master/build/bbl.img

; our ram starts at 0x80000000, so we load the kernel image right here.
kernel_load_base = 0x80000000

; the boot argument delivered to Linux kernel
; earlycon or console must be specified here
bootarg = console=uart8250,mmio,0x10000000

; init ramdisk arguments.
initrd = /root/workspace/busybox-1.31.1/initrd.cpio
initrd_load_base = 0x84000000

[rom]
rom_image = bootrom/bootrom.rv32.img
rom_start = 0x1000
rom_size = 0x1000000


[cpu]
; number of cpus, always required.
nr_cpus = 1
; the cpu index as the primary cpu to boot: optional, default:0
boot_cpu = 0
; the program counter will be set to the value when the hart is reset or poweron
; also the rom image will be loaded to the address
pc_on_reset = 0x4000
; the Core Local Interrupt Controller which generates and delivers machine
; and supervisor timer interrupts to HLIC
clint_base = 0x02000000
clint_size = 0x00010000
; the capacity of TLB, MUST BE POWER OF 2, and 16K is quite enough.
itlb_size = 0x4000
dtlb_size = 0x4000

[mem]
main_memory_start = 0x80000000
main_memory_size_in_mega = 1024

[misc]
; dump the device tree blob, optional
dump_dtb = ./zelda.dtb

[debug]
; the log verbosity is an integer.
;LOG_TRACE = 0, LOG_DEBUG = 1, LOG_INFO = 2, LOG_WARN = 3, LOG_ERROR = 4,
;LOG_FATAL = 5, LOG_UART = 6
verbosity = 2
; initial breakpoints, optional.
;breakpoints = c011de8c
 ```
then run the virtual machine:
```
$./vmm/vmx ./test.vm.ini

[UART16550] bbl loader
[UART16550]               vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
[UART16550]                   vvvvvvvvvvvvvvvvvvvvvvvvvvvv
[UART16550] rrrrrrrrrrrrr       vvvvvvvvvvvvvvvvvvvvvvvvvv
[UART16550] rrrrrrrrrrrrrrrr      vvvvvvvvvvvvvvvvvvvvvvvv
[UART16550] rrrrrrrrrrrrrrrrrr    vvvvvvvvvvvvvvvvvvvvvvvv
[UART16550] rrrrrrrrrrrrrrrrrr    vvvvvvvvvvvvvvvvvvvvvvvv
[UART16550] rrrrrrrrrrrrrrrrrr    vvvvvvvvvvvvvvvvvvvvvvvv
[UART16550] rrrrrrrrrrrrrrrr      vvvvvvvvvvvvvvvvvvvvvv
[UART16550] rrrrrrrrrrrrr       vvvvvvvvvvvvvvvvvvvvvv
[UART16550] rr                vvvvvvvvvvvvvvvvvvvvvv
[UART16550] rr            vvvvvvvvvvvvvvvvvvvvvvvv      rr
[UART16550] rrrr      vvvvvvvvvvvvvvvvvvvvvvvvvv      rrrr
[UART16550] rrrrrr      vvvvvvvvvvvvvvvvvvvvvv      rrrrrr
[UART16550] rrrrrrrr      vvvvvvvvvvvvvvvvvv      rrrrrrrr
[UART16550] rrrrrrrrrr      vvvvvvvvvvvvvv      rrrrrrrrrr
[UART16550] rrrrrrrrrrrr      vvvvvvvvvv      rrrrrrrrrrrr
[UART16550] rrrrrrrrrrrrrr      vvvvvv      rrrrrrrrrrrrrr
[UART16550] rrrrrrrrrrrrrrrr      vv      rrrrrrrrrrrrrrrr
[UART16550] rrrrrrrrrrrrrrrrrr          rrrrrrrrrrrrrrrrrr
[UART16550] rrrrrrrrrrrrrrrrrrrr      rrrrrrrrrrrrrrrrrrrr
[UART16550] rrrrrrrrrrrrrrrrrrrrrr  rrrrrrrrrrrrrrrrrrrrrr
[UART16550]
[UART16550]        INSTRUCTION SETS WANT TO BE FREE
[UART16550] [    0.000000] OF: fdt: Ignoring memory range 0x80000000 - 0x80400000
[UART16550] [    0.000000] Linux version 5.4.0 (root@my-container-host) (gcc version 9.2.0 (GCC)) #49 SMP Thu Feb 20 00:06:15 EST 2020
[UART16550] [    0.000000] earlycon: uart8250 at MMIO 0x0000000010000000 (options '')
[UART16550] [    0.000000] printk: bootconsole [uart8250] enabled
[UART16550] [    0.000000] initrd not found or empty - disabling initrd
[UART16550] [    0.000000] Zone ranges:
[UART16550] [    0.000000]   Normal   [mem 0x0000000080400000-0x00000000bfffffff]
[UART16550] [    0.000000] Movable zone start for each node
[UART16550] [    0.000000] Early memory node ranges
[UART16550] [    0.000000]   node   0: [mem 0x0000000080400000-0x00000000bfffffff]
[UART16550] [    0.000000] Initmem setup node 0 [mem 0x0000000080400000-0x00000000bfffffff]
[UART16550] [    0.000000] elf_hwcap is 0x1101
[UART16550] [    0.000000] percpu: Embedded 12 pages/cpu s18572 r8192 d22388 u49152
[UART16550] [    0.000000] Built 1 zonelists, mobility grouping on.  Total pages: 258570
[UART16550] [    0.000000] Kernel command line: console=uart8250,mmio,0x10000000
[UART16550] [    0.000000] Dentry cache hash table entries: 131072 (order: 7, 524288 bytes, linear)
[UART16550] [    0.000000] Inode-cache hash table entries: 65536 (order: 6, 262144 bytes, linear)
[UART16550] [    0.000000] Sorting __ex_table...
[UART16550] [    0.000000] mem auto-init: stack:off, heap alloc:off, heap free:off
[UART16550] [    0.000000] Memory: 1027168K/1044480K available (4827K kernel code, 167K rwdata, 869K rodata, 194K init, 215K bss, 17312K reserved, 0K cma-reserved)
[UART16550] [    0.000000] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
[UART16550] [    0.000000] rcu: Hierarchical RCU implementation.
[UART16550] [    0.000000] rcu:     RCU event tracing is enabled.
[UART16550] [    0.000000] rcu:     RCU restricting CPUs from NR_CPUS=8 to nr_cpu_ids=1.
[UART16550] [    0.000000] rcu: RCU calculated value of scheduler-enlistment delay is 25 jiffies.
[UART16550] [    0.000000] rcu: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=1
[UART16550] [    0.000000] NR_IRQS: 0, nr_irqs: 0, preallocated irqs: 0
[UART16550] [    0.000000] riscv_timer_init_dt: Registering clocksource cpuid [0] hartid [0]
[UART16550] [    0.000000] clocksource: riscv_clocksource: mask: 0xffffffffffffffff max_cycles: 0x24e6a1710, max_idle_ns: 440795202120 ns
[UART16550] [    0.422538] sched_clock: 64 bits at 10MHz, resolution 100ns, wraps every 4398046511100ns
[UART16550] [    4.252737] Console: colour dummy device 80x25
[UART16550] [    6.235356] Calibrating delay loop (skipped), value calculated using timer frequency.. 20.00 BogoMIPS (lpj=40000)
[UART16550] [    8.003012] pid_max: default: 32768 minimum: 301
[UART16550] [   20.840875] Mount-cache hash table entries: 2048 (order: 1, 8192 bytes, linear)
[UART16550] [   22.340268] Mountpoint-cache hash table entries: 2048 (order: 1, 8192 bytes, linear)
[UART16550] [  122.987299] rcu: Hierarchical SRCU implementation.
[UART16550] [  145.745366] smp: Bringing up secondary CPUs ...
[UART16550] [  146.636491] smp: Brought up 1 node, 1 CPU
[UART16550] [  167.172520] devtmpfs: initialized
[UART16550] [  225.544304] random: get_random_u32 called from bucket_table_alloc.isra.0+0x74/0x1e8 with crng_init=0
[UART16550] [  244.717649] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 7645041785100000 ns
[UART16550] [  246.249764] futex hash table entries: 256 (order: 2, 16384 bytes, linear)
[UART16550] [  264.165035] NET: Registered protocol family 16
[UART16550] [  757.462322] vgaarb: loaded
[UART16550] [  805.868283] clocksource: Switched to clocksource riscv_clocksource
[UART16550] [ 1383.052807] NET: Registered protocol family 2
[UART16550] [ 1401.327814] tcp_listen_portaddr_hash hash table entries: 512 (order: 0, 6144 bytes, linear)
[UART16550] [ 1402.666607] TCP established hash table entries: 8192 (order: 3, 32768 bytes, linear)
[UART16550] [ 1404.833819] TCP bind hash table entries: 8192 (order: 4, 65536 bytes, linear)
[UART16550] [ 1407.025060] TCP: Hash tables configured (established 8192 bind 8192)
[UART16550] [ 1411.010948] UDP hash table entries: 512 (order: 2, 16384 bytes, linear)
[UART16550] [ 1413.035057] UDP-Lite hash table entries: 512 (order: 2, 16384 bytes, linear)
[UART16550] [ 1422.205886] NET: Registered protocol family 1
[UART16550] [ 1425.305232] PCI: CLS 0 bytes, default 64
[UART16550] [ 1482.448305] workingset: timestamp_bits=30 max_order=18 bucket_order=0
[UART16550] [ 2485.278226] Block layer SCSI generic (bsg) driver version 0.4 loaded (major 254)
[UART16550] [ 2486.281572] io scheduler mq-deadline registered
[UART16550] [ 2487.188251] io scheduler kyber registered
[UART16550] [ 6241.054490] Serial: 8250/16550 driver, 4 ports, IRQ sharing disabled
[UART16550] [ 6330.096069] 10000000.uart: ttyS0 at MMIO 0x10000000 (irq = 0, base_baud = 230400) is a 16550A
[UART16550] [ 6332.383725] printk: console [ttyS0] enabled
[UART16550] [ 6332.383725] printk: console [ttyS0] enabled
[UART16550] [ 6333.331288] printk: bootconsole [uart8250] disabled
[UART16550] [ 6333.331288] printk: bootconsole [uart8250] disabled
[UART16550] [ 6735.440324] loop: module loaded
[UART16550] [ 6836.199201] NET: Registered protocol family 10
[UART16550] [ 6869.120954] Segment Routing with IPv6
[UART16550] [ 6872.019522] sit: IPv6, IPv4 and MPLS over IPv4 tunneling driver
[UART16550] [ 6913.590145] NET: Registered protocol family 17
[UART16550] [ 6941.977875] VFS: Cannot open root device "(null)" or unknown-block(0,0): error -6
[UART16550] [ 6942.668342] Please append a correct "root=" boot option; here are the available partitions:
[UART16550] [ 6943.892260] Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(0,0)
[UART16550] [ 6945.071321] CPU: 0 PID: 1 Comm: swapper/0 Not tainted 5.4.0 #49
[UART16550] [ 6945.891648] Call Trace:
[UART16550] [ 6947.070050] [<c0033178>] walk_stackframe+0x0/0x108
[UART16550] [ 6948.207483] [<c00333b8>] show_stack+0x3c/0x50
[UART16550] [ 6949.439245] [<c04c42cc>] dump_stack+0x88/0xb4
[UART16550] [ 6950.699945] [<c00399e8>] panic+0x11c/0x2bc
[UART16550] [ 6951.984874] [<c0001430>] mount_block_root+0x270/0x330
[UART16550] [ 6954.095673] [<c0001584>] mount_root+0x94/0xac
[UART16550] [ 6955.203056] [<c00016ec>] prepare_namespace+0x150/0x1ac
[UART16550] [ 6956.397539] [<c0000f8c>] kernel_init_freeable+0x1dc/0x214
[UART16550] [ 6957.571689] [<c04e04b0>] kernel_init+0x1c/0x11c
[UART16550] [ 6958.882195] [<c0031dd0>] ret_from_exception+0x0/0x10
[UART16550] [ 6959.950079] ---[ end Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(0,0) ]---
```
## How to debug?
The vmm provides a builtin debugger to assist troubleshooting. which is pretty enough for me to find what's going on with my guest. of course you also need the binutils. 

any `ebreak` instruction will cause the vmm entering debug shell and you can also specify the breakpoints(virtual/physical address) in the vm config file.

once a breakpoint is hit in any case, vmm will suspend execution of vmm and prompt an interactive shell:
```
(zelda.risc-v.dbg: 0xc011de8c) help
supported commands:
	info registers  dump the registers of a hart
	info translation  dump the items in translation cache
	info breakpoints  dump all the breakpoints
	continue  continue to execute util it reaches next breakpoint
	break  add a break by following the address of the target address
	/x  dump physical memory segment
	/v  dump virtual memory segment(BE CAUTIOUS TO USE IT !!!)
	backtrace  dump the calling stack...
	help  display all the supported commands
```
### Dump registers information
This dumps all the 32 registers and PC and several important CSRs.
```
(zelda.risc-v.dbg: 0xc011de8c) info registers
[breakpoint at 0xc011de8c]:
dump hart:0x1919080
	hart-id: 0
	pc: 0xc011de8c
	X00(zero): 0x00000000  	X01(ra  ): 0xc011ea14  	X02(sp  ): 0xfec65bc0  	X03(gp  ): 0xc05ea7b8
	X04(tp  ): 0xfec60000  	X05(t0  ): 0xfed24bd0  	X06(t1  ): 0x00000000  	X07(t2  ): 0x00000ff0
	X08(s0  ): 0xfec65c90  	X09(s1  ): 0x00000000  	X10(a0  ): 0xfed24bd0  	X11(a1  ): 0x00000000
	X12(a2  ): 0xfec65cb8  	X13(a3  ): 0x00000004  	X14(a4  ): 0x00100cca  	X15(a5  ): 0x00000000
	X16(a6  ): 0xfec65cb8  	X17(a7  ): 0x00000001  	X18(s2  ): 0x00000000  	X19(s3  ): 0xfec65d90
	X20(s4  ): 0x00000000  	X21(s5  ): 0xfed24cc0  	X22(s6  ): 0xfec199c0  	X23(s7  ): 0x00001000
	X24(s8  ): 0x00001000  	X25(s9  ): 0x00000000  	X26(s10 ): 0xc04eec40  	X27(s11 ): 0xc0561000
	X28(t3  ): 0x00000000  	X29(t4  ): 0x00000000  	X30(t5  ): 0x00000000  	X31(t6  ): 0xfec02e1c
	hart control and status:
	privilege level:1
	status: uie:0 sie:1 mie:0 upie:0 spie:0 mpie:1 spp:1, mpp:0
	interrupt pending: 0x00000000
	interrupt enable: 0x000002aa
	interrupt delegation: 0x00000222
	machine exception delegation: 0x0000b109
```
### Dump translation cache
To see what's inside the translation cache:
```
(zelda.risc-v.dbg: 0xc011de8c) info translation
hart:0 has 45 items in translation cache:
	0xc01043c0: 0x7ff6a0a65000 	0xc01043c4: 0x7ff6a0a65062 	0xc011de50: 0x7ff6a0a6587d 	0xc011de54: 0x7ff6a0a658df
	0xc011de58: 0x7ff6a0a65955 	0xc011de5c: 0x7ff6a0a659cb 	0xc011de60: 0x7ff6a0a65a41 	0xc011de64: 0x7ff6a0a65ab7
	0xc011de68: 0x7ff6a0a65b2d 	0xc011de6c: 0x7ff6a0a65ba3 	0xc011de70: 0x7ff6a0a65c19 	0xc011de74: 0x7ff6a0a65c8f
	0xc011de78: 0x7ff6a0a65d05 	0xc011de7c: 0x7ff6a0a65d7b 	0xc011de80: 0x7ff6a0a65df1 	0xc011de84: 0x7ff6a0a65e67
	0xc011de88: 0x7ff6a0a65edd 	0xc011de8c: 0x7ff6a0a65f3f 	0xc011de90: 0x7ff6a0a65fa1 	0xc011de94: 0x7ff6a0a66003
	0xc011de98: 0x7ff6a0a66065 	0xc011de9c: 0x7ff6a0a660db 	0xc011dea0: 0x7ff6a0a6613d 	0xc011dea4: 0x7ff6a0a661bb
	0xc011dea8: 0x7ff6a0a6621d 	0xc011deac: 0x7ff6a0a6627f 	0xc011deb0: 0x7ff6a0a662e1 	0xc011e9cc: 0x7ff6a0a650e2
	0xc011e9d0: 0x7ff6a0a65160 	0xc011e9d4: 0x7ff6a0a651c5 	0xc011e9d8: 0x7ff6a0a6522c 	0xc011e9dc: 0x7ff6a0a652aa
	0xc011e9e0: 0x7ff6a0a6530c 	0xc011e9e4: 0x7ff6a0a65374 	0xc011e9e8: 0x7ff6a0a653d6 	0xc011e9ec: 0x7ff6a0a65438
	0xc011e9f0: 0x7ff6a0a654a8 	0xc011e9f4: 0x7ff6a0a6550a 	0xc011e9f8: 0x7ff6a0a65580 	0xc011e9fc: 0x7ff6a0a655f6
	0xc011ea00: 0x7ff6a0a65658 	0xc011ea04: 0x7ff6a0a656d6 	0xc011ea08: 0x7ff6a0a65738 	0xc011ea0c: 0x7ff6a0a6579a
	0xc011ea10: 0x7ff6a0a65818
```
### Manipulate breakpoints
the vmm allows you to dynamically add a breakpoint and dump all the breakpoints.
```
(zelda.risc-v.dbg: 0xc011de8c) break 0xc011de90
adding breakpoint: 0xc011de90 succeeds
(zelda.risc-v.dbg: 0xc011de8c) info breakpoints
There are 2 breakpoints:
0xc011de8c    0xc011de90
```

### Dump physical memory
dump the physical memory segment with given addresses range.

**Caveats: this may produce side effect if you are dumping the mmio memory range**
```
(zelda.risc-v.dbg: 0xc011de8c) /x 0x80000000, 0x80000100
host memory range:[0x7ff65f198000 - 0x7ff65f198000]
0x80000000: 1f80006f 34011173 1a010863 02a12423 02b12623 342025f3 0805d263 00159593
0x80000020: 00e00513 02b51263 08000513 30453073 02000513 34452073 02812503 02c12583
0x80000040: 34011173 30200073 00600513 18b51263 08012503 00052023 0ff0000f 08410513
0x80000060: 0805252f 00157593 00058463 34416073 00257593 00058463 0000100f 00457593
0x80000080: 00058463 12000073 00857593 00058663 10500073 ff5ff06f fa1ff06f 00112223
0x800000a0: 00312623 00412823 00512a23 0000c297 00612c23 00259313 00712e23 00628333
0x800000c0: 02812023 f5432303 02912223 00010513 02c12823 34102673 02d12a23 340012f3
0x800000e0: 02e12c23 02f12e23 05012023 05112223 05212423 05312623 05412823 05512a23
```

### Dump virtual memory
dump the virtual memory segment with given virtual addresses range.

**Caveats: this involves the address translation in MMU, this is really dangerous:if the TLB miss, page walker will be started, if paging is missing, an exception will be raised to guest. and also side effect will be encountered if the backing is MMIO, so be cautious to use it**
```
(zelda.risc-v.dbg: 0xc011de8c) /v 0xc011de8c 0xc011deb4
virtual memory range:[c011de8c - c011deb4]
0xc011de8c: 00050c93 9300050c 84930005 06849300 00068493 13000684 85130006 e6851300
0xc011deb4: 
```

### Dump calling stack

**Caveats: in a leaf function, the `ra` of previous frame is not kept in stack. it's mandatory whether current frame is leaf function, MMU side effect is also taken**
```
(zelda.risc-v.dbg: 0xc011de8c) backtrace noleaf 6
dump the calling stack:
	is current frame marked as leaf: no
	maximum frames: 6
	#0 c011de8c
	#1 c011ea14
	#2 c01043c8
	#3 c0108a10
	#4 c0108bd8
	#5 c0163ec8
```
### Resume execution

execution is resumed until next breakpoint is hit
```
(zelda.risc-v.dbg: 0xc011de8c) continue
(zelda.risc-v.dbg: 0xc011de8c)
```


## How is the device tree organized?

the device tree is generated by VMM, finally, its passed to bootrom and then BBL&Linux kernel.
if you specify the option to dump the device tree. you are allowed to inspect what exactly it is by runing:
```
#fdtdump zelda.dtb

**** fdtdump is a low-level debugging tool, not meant for general use.
**** If you want to decompile a dtb, you probably want
****     dtc -I dtb -O dts <filename>

/dts-v1/;
// magic:		0xd00dfeed
// totalsize:		0x64b (1611)
// off_dt_struct:	0x38
// off_dt_strings:	0x488
// off_mem_rsvmap:	0x28
// version:		17
// last_comp_version:	2
// boot_cpuid_phys:	0x0
// size_dt_strings:	0x1c3
// size_dt_struct:	0x450

/ {
    #address-cells = <0x00000002>;
    #size-cells = <0x00000002>;
    compatible = "riscv-virtio";
    model = "riscv-virtio,qemu";
    bootrom@1000 {
        device_type = "bootrom";
        compatible = "zelda 16MB flash";
        reg = <0x00000000 0x00001000 0x00000000 0x01000000>;
    };
    chosen {
        linux,initrd-end = <0x84203a00>;
        linux,initrd-start = <0x84000000>;
        bootargs = "console=uart8250,mmio,0x10000000";
        stdout-path = "/uart@10000000";
    };
    uart@10000000 {
        interrupts = <0x0000000a>;
        interrupt-parent = <0x0000000d>;
        clock-frequency = <0x00384000>;
        compatible = "ns16550a";
        reg = <0x00000000 0x10000000 0x00000000 0x00000100>;
    };
    memory@80000000 {
        device_type = "memory";
        reg = <0x00000000 0x80000000 0x00000000 0x40000000>;
    };
    cpus {
        #address-cells = <0x00000001>;
        #size_cells = <0x00000000>;
        timebase-frequency = <0x00989680>;
        cpu-map {
            cluster0 {
                core0 {
                    cpu = <0x00000001>;
                };
            };
        };
        cpu@0 {
            phandle = <0x00000001>;
            device_type = "cpu";
            status = "okay";
            compatible = "riscv";
            riscv,isa = "rv32ima";
            mmu-type = "riscv,sv32";
            reg = <0x00000000>;
            interrupt-controller {
                #interrupt-cells = <0x00000001>;
                interrupt-controller;
                phandle = <0x00000002>;
                compatible = "riscv,cpu-intc";
            };
        };
    };
    soc {
        #address-cells = <0x00000002>;
        #size-cells = <0x00000002>;
        compatible = "simple-bus";
        ranges;
        clint@2000000 {
            reg = <0x00000000 0x02000000 0x00000000 0x00010000>;
            compatible = "riscv,clint0";
            interrupts-extended = <0x00000002 0x00000007 0x00000002 0xfd7f0000>;
        };
    };
};
```

## CORE FEATURE
- [X] RV32IAM 
- [X] SV32 SoftMMU
- [X] Boot Berkely Bootloader + Linux (WIP)
- [X] native debuger(live breakpoints)

## Demo
[![asciicast](https://asciinema.org/a/lALhpxEOtLeGzPvnoyTA3AMjH.svg)](https://asciinema.org/a/lALhpxEOtLeGzPvnoyTA3AMjH)

## Unprivileged ISAs
#### RV32I instructions
- [X] LUI rd, imm
- [X] AUIPC rd, offset
- [X] JAL rd, offset
- [X] JALR rd, rs1, offset
- [X] BEQ rs1, rs2, offset
- [x] BNE rs1, rs2, offset
- [x] BLT rs1, rs2, offset
- [x] BGE rs1, rs2, offset
- [x] BLTU rs1, rs2, offset
- [x] BGEU rs1, rs2, offset
- [x] LB rd, offset(rs1)
- [x] LH rd, offset(rs1)
- [x] LW rd, offset(rs1)
- [x] LBU rd, offset(rs1)
- [x] LHU rd, offset(rs1)
- [x] SB rs2, offset(rs1)
- [x] SH rs2, offset(rs1)
- [x] SW rs2, offset(rs1)
- [x] ADDI rd, rs1, imm
- [x] SLTI rd, rs1, imm
- [x] SLTIU rd, rs1, imm
- [x] XORI rd, rs1, imm
- [x] ORI rd, rs1, imm
- [x] ANDI rd, rs1, imm
- [x] SLLI rd, rs1, imm
- [x] SRLI rd, rs1, imm
- [x] SRAI rd, rs1, imm
- [x] ADD rd, rs1, rs2
- [x] SUB rd, rs1, rs2
- [x] OR rd, rs1, rs2
- [x] AND rd, rs1, rs2
- [x] XOR rd, rs1, rs2
- [x] SLL rd, rs1, rs2
- [x] SRL rd, rs1, rs2
- [x] SRA rd, rs1, rs2
- [x] SLT rd, rs1, rs2
- [x] SLTU rd, rs1, rs2
- [x] FENCE pred, succ
- [X] FENCE.I

#### RV32M instructions
- [X] MUL rd, rs1, rs2
- [X] MULH rd, rs1, rs2
- [X] MULHSU rd, rs1, rs2
- [X] MULHU rd, rs1, rs2
- [X] DIV rd, rs1, rs2
- [X] DIVU rd, rs1, rs2
- [X] REM rd, rs1, rs2
- [X] REMU rd, rs1, rs2

#### RV32A instructions

- [X] LR.W aqrl, rd, (rs1)
- [X] SC.W aqrl, rd, rs2,(rs1)
- [X] AMOSWAP.W aqrl, rd, rs2, (rs1) 
- [X] AMOADD.W aqrl, rd, rs2, (rs1) 
- [X] AMOXOR.W aqrl, rd, rs2, (rs1) 
- [X] AMOOR.W aqrl, rd, rs2, (rs1) 
- [X] AMOAND.W aqrl, rd, rs2, (rs1) 
- [X] AMOMIN.W aqrl, rd, rs2, (rs1) 
- [X] AMOMAX.W aqrl, rd, rs2, (rs1) 
- [X] AMOMINU.W aqrl, rd, rs2, (rs1) 
- [X] AMOMAXU.W aqrl, rd, rs2, (rs1)

## supervisor-level instructions

- [X] ECALL
- [X] EBREAK
- [X] SRET
- [X] MRET
- [X] SFENCE.VMA
- [X] WFI 
- [X] CSRRW rd, csr, rs1 
- [X] CSRRS rd, csr, rs1
- [X] CSRRC rd, csr, rs1
- [X] CSRRWI rd, csr, zimm
- [X] CSRRSI rd, csr, zimm
- [X] CSRRCI rd, csr, zimm
