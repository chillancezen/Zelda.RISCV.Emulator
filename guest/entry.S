.section .kernel_init_entry


.extern kernel_init 
.global guest_kernel_entry
guest_kernel_entry:
    li sp, 0x20000
    auipc a0, 0x1
    jal ra, guest_kernel_entry
    //addi sp, sp,-16
    jal  ra, kernel_init
    wfi


