.text
.global _start
.global _irq
_start:
    b reset                 //0
    b undefined_instruction //4
    b software_interrupt    //8
    b prefetch_abort        //c
    b data_abort            //10
    b unused                //14
    b _irq                   //18
    b fiq                   //20

reset:
    #mov sp, #4096

    #ldr   r0, =0x53000000     @ WATCHDOG寄存器地址
    #mov   r1, #0x0                     
    #str   r1, [r0]              @ 写入0，禁止WATCHDOG，否则CPU会不断重启

    #msr cpsr_c, 0xd2
    #mov sp, #3072
    #msr cpsr_c, 0xd3
    bl main
_loop:
    b _loop

_irq:
    sub lr, lr, #4
    stmdb sp!, {r0-r12, lr}
    ldr lr, =irq_ret
    mov r0, sp
    b irq_handler
irq_ret:
    ldmia sp!, {r0-r12, pc}^
    
undefined_instruction:
    mov r0, #1
    mov r1, lr
    ldr pc, =exception_handler
software_interrupt:
    mov r0, #2
    mov r1, lr
    ldr pc, =exception_handler
prefetch_abort:
    mov r0, #3
    sub lr, lr, #4
    mov r1, lr 
    ldr pc, =exception_handler
data_abort:
    sub lr, lr, #8
    mov r0, #4
    mov r1, lr
    ldr pc, =exception_handler
unused:
    mov r0, #5
    ldr pc, =exception_handler
fiq:
    mov r0, #7
    ldr pc, =exception_handler
