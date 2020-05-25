    .text
    .globl _start
    .type _start, @function
_start:
    mov %esp, %ebp
    /* TODO: argc and argv */
    call main

    mov %eax, %ebx
    mov $60, %eax
    int $0x80
