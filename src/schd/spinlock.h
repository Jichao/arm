#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

typedef int spinlock_t;

void spinlock_lock(volatile spinlock_t* lock);
void spinlock_unlock(volatile spinlock_t* lock);

#endif