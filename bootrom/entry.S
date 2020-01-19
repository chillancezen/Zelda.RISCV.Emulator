.section .rom_init_entry


.extern rom_init 

.global rom_entry
rom_entry:

    la sp, __init_stack
    auipc a0, 0x1
    //ebreak
    jal ra, rom_init

1:
    wfi
    j 1b



.section .bss
.space 4096 * 2
.align 16
__init_stack:

