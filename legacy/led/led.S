.text
.global _start
_start:
    # gpbcon: configure gbp8 as output pin
    ldr r0, =0x56000010
    mov r1, #0x15400
    str r1, [r0]

    # gpbdata: write gbp8 0
    ldr r0, =0x56000014
    mov r1, #0x00
    str r1, [r0]

_loop:
    b _loop
