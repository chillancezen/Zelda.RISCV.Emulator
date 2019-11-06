OUTPUT_FORMAT("elf32-littleriscv", "elf32-littleriscv",
	      "elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(guest_kernel_entry)


SECTIONS
{
    . = 0x100000;
    .text           :
    {
        *(.kernel_init_entry*)
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


