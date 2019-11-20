.section .kernel_init_entry


.extern kernel_init 
.global guest_kernel_entry
guest_kernel_entry:
    li sp, 0x20000
    auipc a0, 0x1
    //jal ra, guest_kernel_entry
    //addi sp, sp,-16
    jal ra, 1f
    jal ra, kernel_init
    wfi

1:
    addi sp, sp, -16
    li a3, -220
    slti a5, a3, -221
    li a3, 0xffffff22
    sltiu a6, a3, -221
    jal ra, kernel_init


