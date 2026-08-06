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

- [RISC-V Emulation From Scratch - Part I: Dynamic Instruction Translation](https://chillancezen.github.io/riscv_hypervisor_from_scratch_part1.html)
- [RISC-V Emulation From Scratch - Part II: Memory Management Unit](https://chillancezen.github.io/riscv_hypervisor_from_scratch_part2.html)
- [RISC-V Emulation From Scratch - Part III: Privilege Level and Trap handling](https://chillancezen.github.io/riscv_hypervisor_from_scratch_part3.html)

## The emulated platform

The vmm presents a single-hart rv32ima machine with these devices:

| Address     | Size  | Device                                                  |
|-------------|-------|---------------------------------------------------------|
| `0x00000000`| 4 MiB | boot rom: device tree, boot info block, machine mode firmware |
| `0x00400000`| 4 KiB | `sifive,test0` finisher, used to power the machine off  |
| `0x02000000`| 64 KiB| `riscv,clint0`: `mtime`, `mtimecmp`, `msip`             |
| `0x0c000000`| 64 MiB| `riscv,plic0`: routes device interrupts to the harts    |
| `0x10000000`| 256 B | `ns16550a` uart, wired to PLIC source 10                |
| `0x80000000`| ram   | main memory; the kernel is loaded at `0x80400000`       |

The boot rom holds a machine mode firmware that provides SBI itself, so no
separate bootloader is needed. It handles ecalls from the supervisor,
implements SBI v0.3 (`base`, `time`, `ipi`, `rfence`, `srst`) alongside the
v0.1 legacy calls, and converts the machine timer interrupt it owns into the
supervisor timer interrupt the kernel waits on.

The guest clock is virtual: it advances with the amount of guest code
executed rather than with host wall time. Tying it to the host would give the
guest a machine that runs a handful of instructions per timer tick, and the
kernel would spend its whole life inside the timer handler. `[cpu]
ticks_per_translation_unit` in the config file sets the exchange rate.

## How to build?

Host env: a x86_64 Linux host with gcc, plus a riscv cross compiler for the
firmware. On Debian or Ubuntu:

```
# apt install gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
```

A riscv64 cross compiler targets rv32 fine; if you have a native rv32
toolchain, point `CROSS_COMPILE` at it instead. Then:

```
$ make            # the emulator and its machine mode firmware, a few seconds
$ make guest      # downloads and builds an rv32 Linux, several minutes
```

`make guest` fetches a kernel tarball, configures it for this platform
(`guest/zelda.config`), builds a small freestanding init into an initramfs,
and leaves the result at `guest/Image`. The config keeps the guest inside the
instruction set the emulator implements: plain rv32ima, with the compressed
and floating point extensions turned off.

## How to run ?
Now you should be able to find an executable:`vmx` under directory vmm. that's the virtual machine monitor.
in order to run a guest, we have to define a configuration file to instruct vmm how to load and run a guest. 

The machine is described by a config file; **test.vm.ini** is a working one
and documents every key it accepts. The settings worth knowing about:

| Key | Meaning |
|-----|---------|
| `image.kernel` | raw rv32 Linux image, loaded verbatim |
| `image.kernel_load_base` | where it goes, and where the firmware enters it; must be 4 MiB aligned |
| `image.bootarg` | kernel command line |
| `image.initrd` | optional external ramdisk; unnecessary when the kernel carries its own initramfs |
| `rom.rom_start` / `rom.rom_size` | must be `0x0` and at least 4 MiB, see the layout note in the file |
| `cpu.ticks_per_translation_unit` | how fast the guest clock runs relative to guest progress |
| `mem.main_memory_*` | size and placement of ram |
| `debug.verbosity` | 0 is a full instruction trace, 4 shows only errors |

then run the virtual machine:
```
$ ./vmm/vmx test.vm.ini

zelda machine mode firmware
  platform    : rv32ima, 1 hart(s)
  memory      : 0x80000000 + 256 MiB
  device tree : 0x1000
  kernel      : 0x80400000
  sbi         : v0.3, extensions base/time/ipi/rfence/srst

device tree detected at 0x1000
device tree blob size: 2074 bytes
device tree blob sha1 checksum: 459a24ccd5e60cfb27e6d44e6dfd9e9d4c74d670
[firmware] entering supervisor at 0x80400000

[    0.000000] Linux version 6.1.75 (riscv64-linux-gnu-gcc 11.4.0) #3 SMP
[    0.000000] earlycon: uart8250 at MMIO 0x0000000010000000 (options '')
[    0.000000] printk: bootconsole [uart8250] enabled
[    0.000000] Machine model: riscv-virtio,qemu
[    0.000000] Zone ranges:
[    0.000000]   Normal   [mem 0x0000000080400000-0x000000008fffffff]
[    0.000000] riscv: base ISA extensions aim
[    0.000000] riscv: ELF capabilities aim
[    0.000000] Kernel command line: console=ttyS0 earlycon=uart8250,mmio,0x10000000
[    0.000000] NR_IRQS: 64, nr_irqs: 64, preallocated irqs: 0
[    0.000000] riscv-plic: plic@c000000: mapped 31 interrupts with 1 handlers for 2 contexts.
[    0.000000] clocksource: riscv_clocksource: mask: 0xffffffffffffffff max_cycles: 0x24e6a1710
[    0.842022] devtmpfs: initialized
[    1.666476] printk: console [ttyS0] enabled
[    2.343110] Freeing unused kernel image (initmem) memory: 4140K
[    2.343802] Run /init as init process

  Zelda RISC-V emulator -- rv32ima Linux userspace is alive.
  Type 'help' for the builtins, 'poweroff' to stop the machine.

zelda:/# uname
Linux 6.1.75 riscv32
zelda:/# poweroff
powering off
[   72.784343] reboot: Power down
zelda: system halted
```

The console is the terminal the emulator was started from, put into raw mode,
so the guest receives keystrokes directly. Press `ctrl-a` then `x` to stop the
emulator from outside the guest.

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
- [X] RV32IMA
- [X] SV32 SoftMMU
- [X] Machine mode SBI firmware in the boot rom, so no external bootloader
- [X] CLINT, PLIC and a 16550 console with working input
- [X] Interrupt delivery and `wfi`
- [X] Boots Linux to an interactive userspace shell
- [X] native debuger(live breakpoints)

Not implemented: multiple harts run concurrently (the vmm executes one hart),
the compressed and floating point extensions, and any block or network device.
The page walker does not enforce the U, R, W and X permission bits or update
the accessed and dirty bits.

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
