    .section .text

    .global context_switch
    .type context_switch, @function

context_switch:
    mov 4(%esp), %eax /* newesp */
    mov 8(%esp), %ecx /* saveesp */
    mov 12(%esp), %edx /* as */

    push %ebx
    push %esi
    push %edi
    push %ebp

    mov %esp, (%ecx) /* save esp */
    mov %eax, %esp   /* load new esp */
    mov %edx, %cr3   /* switch page tables */

    pop %ebp
    pop %edi
    pop %esi
    pop %ebx

    ret /* exploiting ret to resume the process */


    .global fork_switchable
    .type fork_switchable, @function

fork_switchable:
    mov 4(%esp), %eax /* saveesp*/
    mov 8(%esp), %ecx /* ias */
    mov 12(%esp), %edx /* as */

    push %ebx
    push %esi
    push %edi
    push %ebp

    mov %esp, (%eax) /* save esp */

    push %edx
    push %ecx
    call fork_proc_addr_space
    pop %ecx
    pop %edx

    mov %eax, (%edx)

    pop %ebp
    pop %edi
    pop %esi
    pop %ebx

    ret /* compatible frame with context_switch */
