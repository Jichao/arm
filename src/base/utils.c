#include "utils.h"
#include "stdio.h"

uint32_t jround(uint32_t value, uint32_t align, bool up)
{
    if (up) {
        return (value + align - 1) / align * align;
    }
    else {
        return value / align * align;
    }
}

int roundf(float a)
{
    int c = a + 0.5;
    if (c > a) {
        return c;
    }
    else {
        return c - 1;
    }
}

// https://stackoverflow.com/questions/13881487/should-i-worry-about-the-alignment-during-pointer-casting
//一定要注意不能够随便cast指针啊。
uint32_t buff_to_u32(unsigned char *buff)
{
    return buff[0] + (buff[1] << 8) + (buff[2] << 16) + (buff[3] << 24);
}

void abort(void) { printf("!!!!abort\r\n"); }

void dump_current_context(void)
{
    uint32_t* regs = nullptr;
    uint32_t cpsr;
    __asm__ (
        "stmdb sp!, {r0-r12,r14-r15}\n"
        "mov %0, sp\n"
        "mrs %1, cpsr"
        :"=r"(regs),"=r"(cpsr)
        :
        :"sp"
    );

    printk("r0: 0x%x r1: 0x%x r2: 0x%x r3: 0x%x r4: 0x%x\r\n", regs[0], regs[1],
           regs[2], regs[3], regs[4]);
    printk("r5: 0x%x r6: 0x%x r7: 0x%x r8: 0x%x r9: 0x%x\r\n", regs[5], regs[6],
           regs[7], regs[8], regs[9]);
    printk("r10: 0x%x r11: 0x%x r12: 0x%x r13: 0x%x r14: 0x%x\r\n", regs[10], regs[11],
           regs[12], regs - 64, regs[13]);
    printk("pc: 0x%x cpsr: 0x%x\r\n", regs[14], cpsr);

    __asm__ (
        "add sp, sp, #60\n"
        :::"sp"
    );
}