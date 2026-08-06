OUTPUT_FORMAT("elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(rom_entry)


SECTIONS
{
    /*
     * The rom region starts at 0x1000. Its first pages are spoken for:
     *   0x1000 - 0x3000  device tree blob written by the vmm
     *   0x3000 - 0x4000  boot info handoff block (see zelda_boot_info.h)
     * so the firmware image itself begins at 0x4000, which is also the value
     * the vmm resets the program counter to.
     */
    . = 0x4000;
    .text           :
    {
        *(.rom_init_entry*)
        *(.text*)
        *(.rodata*)
    }
    .data           :
    {
        KEEP(*( .init_array ));
        KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
        *(.data*)
        *(.sdata*)
    }

    /*
     * Nothing clears rom for us, so the firmware zeroes this range itself on
     * reset. The symbols have to be word aligned: the clearing loop stores a
     * word at a time.
     */
    . = ALIGN(4);
    __bss_start = .;
    .bss            :
    {
        *(.sbss*)
        *(.bss*)
        *(COMMON)
    }
    . = ALIGN(4);
    __bss_end = .;

    /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) *(.comment) }
}
