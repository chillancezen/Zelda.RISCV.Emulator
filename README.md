# Zelda.RISCV.Emulator
The system level emulator which utilizes dynamic binary translation for RISCV ISA
```
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
[UART16550] Zone ranges:
[UART16550]   Normal   [mem 0x0000000080400000-0x00000000bfffffff]
[UART16550] Movable zone start for each node
[UART16550] Early memory node ranges
[UART16550]   node   0: [mem 0x0000000080400000-0x00000000bfffffff]
[UART16550] Initmem setup node 0 [mem 0x0000000080400000-0x00000000bfffffff]
[UART16550] On node 0 totalpages: 261120
[UART16550]   Normal zone: 2550 pages used for memmap
[UART16550]   Normal zone: 0 pages reserved
[UART16550]   Normal zone: 261120 pages, LIFO batch:63
[UART16550] elf_hwcap is 0x1101
[UART16550] percpu: Embedded 12 pages/cpu s18572 r8192 d22388 u49152
[UART16550] pcpu-alloc: s18572 r8192 d22388 u49152 alloc=12*4096
[UART16550] pcpu-alloc: [0] 0
[UART16550] Built 1 zonelists, mobility grouping on.  Total pages: 258570
[UART16550] Kernel command line: console=tty0
[UART16550] Dentry cache hash table entries: 131072 (order: 7, 524288 bytes, linear)
[UART16550] Inode-cache hash table entries: 65536 (order: 6, 262144 bytes, linear)
[UART16550] Sorting __ex_table...
[UART16550] mem auto-init: stack:off, heap alloc:off, heap free:off
[UART16550] Memory: 1027164K/1044480K available (4827K kernel code, 167K rwdata, 869K rodata, 194K init, 215K bss, 17316K reserved, 0K cma-reserved)
[UART16550] SLUB: HWalign=64, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
[UART16550] rcu: Hierarchical RCU implementation.
[UART16550] rcu: 	RCU event tracing is enabled.
[UART16550] rcu: 	RCU restricting CPUs from NR_CPUS=8 to nr_cpu_ids=1.
[UART16550] rcu: RCU calculated value of scheduler-enlistment delay is 25 jiffies.
[UART16550] rcu: Adjusting geometry for rcu_fanout_leaf=16, nr_cpu_ids=1
[UART16550] NR_IRQS: 0, nr_irqs: 0, preallocated irqs: 0
[UART16550] riscv_timer_init_dt: Registering clocksource cpuid [0] hartid [0]
[UART16550] clocksource: riscv_clocksource: mask: 0xffffffffffffffff max_cycles: 0x24e6a1710, max_idle_ns: 440795202120 ns
[UART16550] sched_clock: 64 bits at 10MHz, resolution 100ns, wraps every 4398046511100ns
[UART16550] Console: colour dummy device 80x25
[UART16550] printk: console [tty0] enabled
[UART16550] Calibrating delay loop (skipped), value calculated using timer frequency.. 20.00 BogoMIPS (lpj=40000)
[UART16550] pid_max: default: 32768 minimum: 301
[UART16550] Mount-cache hash table entries: 2048 (order: 1, 8192 bytes, linear)
[UART16550] Mountpoint-cache hash table entries: 2048 (order: 1, 8192 bytes, linear)
[UART16550] before: arch_call_rest_init
[UART16550] rcu: Hierarchical SRCU implementation.
[UART16550] smp: Bringing up secondary CPUs ...
[UART16550] smp: Brought up 1 node, 1 CPU
[UART16550] devtmpfs: initialized
[UART16550] random: get_random_u32 called from bucket_table_alloc.isra.0+0x74/0x1e8 with crng_init=0
[UART16550] clocksource: jiffies: mask: 0xffffffff max_cycles: 0xffffffff, max_idle_ns: 7645041785100000 ns
[UART16550] futex hash table entries: 256 (order: 2, 16384 bytes, linear)
[UART16550] NET: Registered protocol family 16
[UART16550] vgaarb: loaded
[UART16550] Oops - illegal instruction [#1]
[UART16550] dules linked in:
[UART16550] U: 0 PID: 11 Comm: migration/0 Not tainted 5.4.0 #43
[UART16550] sepc: c00d0a9c ra : c00d0b50 sp : fec7fe80
[UART16550]  gp : c05ea7b8 tp : fec80000 t0 : fec03760
[UART16550]  t1 : 0000038e t2 : 0000ffff s0 : fec7fe90
[UART16550]  s1 : fec65e5c a0 : c05eaad4 a1 : 00000000
[UART16550]  a2 : 00000000 a3 : 00000001 a4 : 00000000
[UART16550]  a5 : 00000000 a6 : 00000000 a7 : 00000000
[UART16550]  s2 : 00000000 s3 : ffffffff s4 : c05eaad4
[UART16550]  s5 : 00000003 s6 : 00000002 s7 : 00000004
[UART16550]  s8 : 00000001 s9 : 00000001 s10: 00000000
[UART16550]  s11: fec65e48 t3 : ffffffff t4 : ffffffff
[UART16550]  t5 : 000019d3 t6 : 00000000
[UART16550] sstatus: 00000120 sbadaddr: 00000000 scause: 00000002
[UART16550] ---[ end trace f1ba0ddfcd88fd78 ]---
[UART16550] note: migration/0[11] exited with preempt_count 1
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

## supervisor-level ISAs

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
