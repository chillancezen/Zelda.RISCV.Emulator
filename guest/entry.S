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
    li a3, 0xbeefdead
    xori a0, a3, 0xd
    ori a1, a3, 0x50
    andi a4, a3, 0xff
    slli a5, a3, 31
    srli a6, a3, 28
    srai a7, a3, 4
    jal test_procedure
    jal ra, kernel_init
    li a0, 0xb8000
    lw a1, (a0)


    li a1, 0x8f008181
    sw a1, (a0)

    lh a2, (a0)
    lhu a3, (a0)

    lb a4, (a0)
    lbu a5, (a0)
    li a5, 0x123
    bne a5, a5, 2f
2:
    li a4, 0x12345
    li a5, 0x10001
    add zero, a4, a5
    sub a6, a5, a4
    li a5, 0xff00
    and a7, a5, a4
    li a4, 0x10001
    li a5, 0xffff
    or a6, a4, a5
    xor a7, a4, a5
    li a0, -0x124
    li a1, -0x123
    sltu a2, a0, a1
    li a0, 0xbeefdead
    li a1, 33
    sra a2, a0, a1
    fence.i
    fence.i
    jal ra, test_procedure

    li a3, 1234
    li a4, -1000
    mul a5, a3, a4
    mulh a6, a3, a4
    mulhu a7, a3, a4
    mulhsu a2, a4, a3

    li a0, 0xff000000
    li a1, 0xff000000
    mulhsu a2, a0, a1
    
    li a0, -1 << 31
    li a1, 1
    div a2, a0, a1

    li a0, -1<<31
    li a1, -1
    divu a2, a0, a1

    li a0, -1<<31
    li a1, 1
    rem a2, a0, a1

    li a0, -1<<31
    li a1, -1
    remu a2, a0, a1
    addi a5,a5,-0x1

    li a0, 0x00000003
    li a1, 0x00000007
    sub a2, a0, a1


    li a0, 0x12345
    csrw mscratch, a0
    //ebreak
    j 1f
1:
    wfi

test_procedure:
    addi sp, sp, -16
    ret

.global raw_puts_another
raw_puts_another:
    addi    sp,sp,-48
    sw  s0,44(sp)
    addi    s0,sp,48
    sw  a0,-36(s0)
    lw  a5,-36(s0)
    sw  a5,-20(s0)
    sb  zero,-21(s0)
    j   2f
1:
    lw  a5,-20(s0)
    lbu a5,0(a5)
    sb  a5,-21(s0)
    lui a5,0x10000
    lbu a4,-21(s0)
    sb  a4,0(a5) 
    lw  a5,-20(s0)
    addi    a5,a5,1
    sw  a5,-20(s0)
2:
    lw  a5,-20(s0)
    #ebreak
    lbu a5,0(a5)
    #ebreak
    bnez    a5, 1b
    nop
    nop
    lw  s0,44(sp)
    addi    sp,sp,48
    ret
