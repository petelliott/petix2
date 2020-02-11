    .section .text

    .global irq0
irq0:
    pushal
    pushw $0
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq1
irq1:
    pushal
    pushw $1
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq2
irq2:
    pushal
    pushw $2
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq3
irq3:
    pushal
    pushw $3
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq4
irq4:
    pushal
    pushw $4
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq5
irq5:
    pushal
    pushw $5
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq6
irq6:
    pushal
    pushw $6
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq7
irq7:
    pushal
    pushw $7
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq8
irq8:
    pushal
    pushw $8
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq9
irq9:
    pushal
    pushw $9
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq10
irq10:
    pushal
    pushw $10
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq11
irq11:
    pushal
    pushw $11
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq12
irq12:
    pushal
    pushw $12
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq13
irq13:
    pushal
    pushw $13
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq14
irq14:
    pushal
    pushw $14
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global irq15
irq15:
    pushal
    pushw $15
    pushw $-1
    call general_irq_handler
    add $0x4, %esp
    popal
    iret

    .global excep0
excep0:
    pushw $-1
    pushw $0
    pushal
    call general_exception_handler
    popal
    iret

    .global excep1
excep1:
    pushw $-1
    pushw $1
    pushal
    call general_exception_handler
    popal
    iret

    .global excep2
excep2:
    pushw $-1
    pushw $2
    pushal
    call general_exception_handler
    popal
    iret

    .global excep3
excep3:
    pushw $-1
    pushw $3
    pushal
    call general_exception_handler
    popal
    iret

    .global excep4
excep4:
    pushw $-1
    pushw $4
    pushal
    call general_exception_handler
    popal
    iret

    .global excep5
excep5:
    pushw $-1
    pushw $5
    pushal
    call general_exception_handler
    popal
    iret

    .global excep6
excep6:
    pushw $-1
    pushw $6
    pushal
    call general_exception_handler
    popal
    iret

    .global excep7
excep7:
    pushw $-1
    pushw $7
    pushal
    call general_exception_handler
    popal
    iret

    .global excep8
excep8:
    pushw $-1
    pushw $8
    pushal
    call general_exception_handler
    popal
    iret

    .global excep9
excep9:
    pushw $-1
    pushw $9
    pushal
    call general_exception_handler
    popal
    iret

    .global excep10
excep10:
    pushw $-1
    pushw $10
    pushal
    call general_exception_handler
    popal
    iret

    .global excep11
excep11:
    pushw $-1
    pushw $11
    pushal
    call general_exception_handler
    popal
    iret

    .global excep12
excep12:
    pushw $-1
    pushw $12
    pushal
    call general_exception_handler
    popal
    iret

    .global excep13
excep13:
    pushw $-1
    pushw $13
    pushal
    call general_exception_handler
    popal
    iret

    .global excep14
excep14:
    pushw $-1
    pushw $14
    pushal
    call general_exception_handler
    popal
    iret

    .global excep15
excep15:
    pushw $-1
    pushw $15
    pushal
    call general_exception_handler
    popal
    iret

    .global excep16
excep16:
    pushw $-1
    pushw $16
    pushal
    call general_exception_handler
    popal
    iret

    .global excep17
excep17:
    pushw $-1
    pushw $17
    pushal
    call general_exception_handler
    popal
    iret

    .global excep18
excep18:
    pushw $-1
    pushw $18
    pushal
    call general_exception_handler
    popal
    iret

    .global excep19
excep19:
    pushw $-1
    pushw $19
    pushal
    call general_exception_handler
    popal
    iret

    .global excep20
excep20:
    pushw $-1
    pushw $20
    pushal
    call general_exception_handler
    popal
    iret

    .global excep21
excep21:
    pushw $-1
    pushw $21
    pushal
    call general_exception_handler
    popal
    iret

    .global excep22
excep22:
    pushw $-1
    pushw $22
    pushal
    call general_exception_handler
    popal
    iret

    .global excep23
excep23:
    pushw $-1
    pushw $23
    pushal
    call general_exception_handler
    popal
    iret

    .global excep24
excep24:
    pushw $-1
    pushw $24
    pushal
    call general_exception_handler
    popal
    iret

    .global excep25
excep25:
    pushw $-1
    pushw $25
    pushal
    call general_exception_handler
    popal
    iret

    .global excep26
excep26:
    pushw $-1
    pushw $26
    pushal
    call general_exception_handler
    popal
    iret

    .global excep27
excep27:
    pushw $-1
    pushw $27
    pushal
    call general_exception_handler
    popal
    iret

    .global excep28
excep28:
    pushw $-1
    pushw $28
    pushal
    call general_exception_handler
    popal
    iret

    .global excep29
excep29:
    pushw $-1
    pushw $29
    pushal
    call general_exception_handler
    popal
    iret

    .global excep30
excep30:
    pushw $-1
    pushw $30
    pushal
    call general_exception_handler
    popal
    iret

    .global excep31
excep31:
    pushw $-1
    pushw $31
    pushal
    call general_exception_handler
    popal
    iret
