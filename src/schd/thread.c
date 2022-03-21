#include "schd/thread.h"
#include "mem/kmalloc.h"
#include "thread.h"
#include <string.h>

#define DEFAULT_STACK_SIZE 4096

int _tid = 0;

void thread_exit(void* args)
{
    printk("thread exited\r\n");
}

thread_t* thread_create(thread_func_t entry)
{
    thread_t* thread = (thread_t*)kmalloc(sizeof(thread_t));
    thread->entry = entry;
    thread->state = kThread_New;
    thread->tid = _tid++;
    thread->stack = kzmalloc(DEFAULT_STACK_SIZE);
    thread->stack_size = DEFAULT_STACK_SIZE;
    memset(&thread->context, 0, sizeof(cpu_context_t));
    thread->context.pc = entry;
    thread->context.lr = &thread_exit;
    thread->context.sp = thread->stack + thread->stack_size;
    return thread;
}

void thread_start(thread_t* thread)
{
}

void thread_set_name(thread_t* thread, const char* name)
{
    strncpy(thread->name, name, 8);
    thread->name[7] = '\0';
}

void thread_destroy(thread_t* thread)
{
    kfree(thread->stack);
    kfree(thread);
}

void dump_cpu_context(cpu_context_t* c)
{
    printk("pc: 0x%x lr: 0x%x sp: 0x%x r0: 0x%x r1: 0x%x\r\n", c->pc, c->lr, c->sp, c->regs[0], c->regs[1]);
    printk("r2: 0x%x r3: 0x%x r4: 0x%x r5: 0x%x r6: 0x%x\r\n", c->regs[2], c->regs[3], c->regs[4], c->regs[5], c->regs[6]);
    printk("r7: 0x%x r8: 0x%x r9: 0x%x r10: 0x%x r11: 0x%x\r\n", c->regs[7], c->regs[8], c->regs[9], c->regs[10], c->regs[11]);
    printk("r12: 0x%x \r\n", c->regs[12]);
}

void dump_thread(thread_t* thread)
{
    cpu_context_t* c = &thread->context;
    printk("thread: %p name: %s tid: %d stack:  %p\r\n", thread, thread->name, thread->tid, thread->stack);
    dump_cpu_context(c);
}