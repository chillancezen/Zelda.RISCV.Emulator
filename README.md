# Zelda.RISCV.Emulator
The system level emulator which utilizes dynamic binary translation for RISCV ISA
## How to build?
Host env: a x86_64 Linux host + gcc 4.8.x

there are lots of things to build in order to run a riscv Linux. here is the guide page: https://risc-v-getting-started-guide.readthedocs.io/en/latest/linux-qemu.html
- riscv32 cross-compile toolchain: `riscv32-unknown-linux-gnu-` 
- riscv-Linux, make sure a newer Linux kernel source is built from, Linux 5.4.0 is chosen in my testbed.
- SBI software: Berkely Bootloader(aka. BBL) is my choice.
- initramfs cpio built with busybox.
- build the emulator by simplily running `make`
## how to run ?
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

## what's the device tree organized?

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
