    .section .text
    .global raw_syscall
    .type raw_syscall, @function
raw_syscall:
    push %ebp
    mov %esp, %ebp

    push %ebx
    push %esi
    push %edi

    mov 8(%ebp), %eax
    mov 12(%ebp), %ebx
    mov 16(%ebp), %ecx
    mov 20(%ebp), %edx
    mov 24(%ebp), %esi
    mov 28(%ebp), %edi

    int $0x80

    pop %edi
    pop %esi
    pop %ebx

    mov %ebp, %esp
    pop %ebp

    ret
