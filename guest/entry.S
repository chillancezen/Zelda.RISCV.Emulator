.section .kernel_init_entry

.extern init_main
//.extern kernel_init 
.global guest_kernel_entry
guest_kernel_entry:
    // a0 is the booting hartid
    // a1 is the DTB base
    la sp, __stack_top
    //ebreak
    jal ra, init_main
    jal ra, infinite_loop


infinite_loop:
    wfi
    #li zero, 0x8080808
    j infinite_loop




// allocate an area of memory to serve as initial stack
.section .bss
.space 1024*8
.align 4
__stack_top:
