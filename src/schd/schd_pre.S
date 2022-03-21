.text
.global schd_switch
schd_switch:
    mov r0, sp
    mov r1, lr
    bl schd_switch_imp
    bl restore_irq

    //restore r1-r14
    add r0, r0, #4
    ldmia r0, {r1-r14}
    sub r0, r0, #4

    //restore r0, pc
    push {r0}
    ldr r0, [r0]
    pop {r0}
    ldr pc, [r0, #60]