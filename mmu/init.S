.text
.global _start
_start:
    ldr   r0, =0x53000000     @ WATCHDOG寄存器地址
    mov   r1, #0x0                     
    str   r1, [r0]              @ 写入0，禁止WATCHDOG，否则CPU会不断重启
    mov sp, #4096
    bl main
_loop:
    b _loop
