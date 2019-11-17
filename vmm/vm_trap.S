/*
 * Copyright (c) 2019 Jie Zheng
 */

.section .text
.global vmm_entry_point
vmm_entry_point:
    // FIXME: switch stack during context switching, or the stack of the vmm
    // may be potentially exhausted.
    .extern vmexit
    pushq %r12
    movq %r12, %rdi
    call vmexit

    .extern vmpanic
    popq %r12
    movq %r12, %rdi
    call vmpanic




.global vmm_jumper_begin
.global vmm_jumper_end
vmm_jumper_begin:
    movq $vmm_entry_point, %rax
    jmpq *%rax
vmm_jumper_end:



