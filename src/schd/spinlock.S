.text
.global spinlock_lock
.global spinlock_unlock

spinlock_lock:
    mov r2, #1
    swp r1, r2, [r0]
    cmp r1, #0
    bxeq lr
    b spinlock_lock


spinlock_unlock:
    mov r1, #0
    str r1, [r0]
    bx lr
