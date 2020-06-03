    .text
    .globl _start
    .type _start, @function
_start:
    mov %esp, %ebp

    lea 4(%ebp), %eax
    push %eax
    mov 0(%ebp), %eax
    push %eax

    call main

    mov %eax, %ebx
    mov $60, %eax
    int $0x80
