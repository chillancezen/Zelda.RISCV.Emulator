.section .kernel_init_entry


.extern kernel_init 
.global guest_kernel_entry
guest_kernel_entry:
    li sp, 0x20000
    jal  ra, kernel_init
    wfi


