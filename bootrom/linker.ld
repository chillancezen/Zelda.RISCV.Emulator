OUTPUT_FORMAT("elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(rom_entry)


SECTIONS
{
    /*
     * ROM actually begins at 0x1000, but first 3 pages are reserved for
     * device tree blob.
     * XXX: that's to say, size of DTB must not exceed 4096 * 3 bytes.
     * . = 0x1000;
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
    }
    
    .bss            :
    {
        *(.bss*)
    }

    /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) }
}


