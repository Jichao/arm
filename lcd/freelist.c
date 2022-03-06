#include "freelist.h"
#include "common.h"
#include "kmalloc.h"

//分配的时候
// freelist永远是指向下逻辑上下一个free的slot
// free的时候
// freelist看下一个物理上的free slot，如果是已经release了，就直接合并了.(类buddy
// system) 如果不能够合并就直接加进freelist
typedef enum s_entry_type {
    kEntryType_Logic, //逻辑上边下一个
    kEntryType_Phy,   //物理上边下一个，可能跟logic不同
} entry_type_t;

extern int _memerr;

static BOOL is_entry_free(freelist_entry_t *freelist)
{
    return !((uint32_t)freelist->next & 1);
}

static void set_entry_free(freelist_entry_t *freelist, BOOL free)
{
    if (free) {
        freelist->next = (freelist_entry_t *)((uint32_t)freelist->next & ~1);
    } else {
        freelist->next = (freelist_entry_t *)((uint32_t)freelist->next | 1);
    }
}

static freelist_entry_t *get_next_entry(freelist_entry_t *entry,
                                        entry_type_t type)
{
    kassert(entry, "invalid entry : %p\r\n", entry);
    if (type == kEntryType_Logic) {
        return (freelist_entry_t *)((uint32_t)entry->next & ~1);
    }
    else {
        freelist_entry_t *next_entry =
            (freelist_entry_t *)((char *)entry + entry->len +
                                 sizeof(freelist_entry_t));
        return next_entry;
    }
}

static void set_next_entry(freelist_entry_t *curr, freelist_entry_t *next)
{
    kassert(curr && next && is_entry_free(curr) && is_entry_free(next),
            "invalid entry: all entries should be free");
    curr->next = next;
    set_entry_free(curr, TRUE);
}

static BOOL is_entry_tail(freelist_entry_t *freelist)
{
    return (get_next_entry(freelist, kEntryType_Logic) == NULL);
}

freelist_entry_t *freelist_new(void)
{
    int init_size = 4 << 20;
    freelist_entry_t *entry = (freelist_entry_t *)kmalloc(init_size);
    entry->next = NULL;
    entry->len = init_size - sizeof(freelist_entry_t);
    set_entry_free(entry, TRUE);
    return entry;
}

freelist_entry_t *new_sub_entry(freelist_entry_t *freelist, int size)
{
    int olen = freelist->len;
    if (olen - size > sizeof(freelist_entry_t)) {
        freelist->len = olen - size - sizeof(freelist_entry_t);
        freelist_entry_t *sub_entry = get_next_entry(freelist, kEntryType_Phy);
        sub_entry->len = size;
        set_entry_free(sub_entry, FALSE);
    }
    else {
        return NULL;
    }
}

freelist_entry_t *get_free_entry(freelist_entry_t *entry, int size,
                                 freelist_entry_t **prev_list)
{
    kassert(entry, "invalid free entry %p\r\n", entry);
    *prev_list = NULL;
    for (; !is_entry_tail(entry); *prev_list = entry, entry = get_next_entry(entry, kEntryType_Logic)) {
        //dprintk("entry free %d size = %d, need size %d\n", is_entry_free(entry), entry->len, size);
        if (is_entry_free(entry) && entry->len >= size) {
            return entry;
        }
    }
    return NULL;
}

void *fmalloc(freelist_man_t *man, int size)
{
    kassert(man, "invalid man");
    kassert((size > 16 && size < ((4 << 20) - sizeof(freelist_entry_t))), "invalid size");

    size = jround(size, 4, TRUE);
    freelist_entry_t *prev = NULL;
    freelist_entry_t *next = NULL;
    freelist_entry_t *curr = get_free_entry(man->head, size, &prev);
    if (curr) {
        next = get_next_entry(curr, kEntryType_Logic);
        freelist_entry_t *sub_entry = new_sub_entry(curr, size);
        //能够split出来
        if (sub_entry) {
            return (char *)sub_entry + sizeof(freelist_entry_t);
        }
        else {
            if (prev) {
                set_next_entry(prev, next);
            }
            else {
                set_next_entry(man->head, next);
            }
            set_entry_free(curr, FALSE);
            return (char *)curr + sizeof(freelist_entry_t);
        }
    }
    else {
        //存在freelist但是size小了，或者不存freelist了。
        //无论何种情况都需要新申请空间了。
        freelist_entry_t* new_entry = freelist_new();
        freelist_entry_t *sub_entry = new_sub_entry(new_entry, size);
        set_next_entry(new_entry, man->head);
        man->head = new_entry;
        return (char *)sub_entry + sizeof(freelist_entry_t);
    }
}

void ffree(freelist_man_t *man, void *ptr) 
{
    freelist_entry_t* curr = (freelist_entry_t*)((char*)ptr - sizeof(freelist_entry_t));
    kassert(!is_entry_free(curr), "invalid free pointer: %p\r\n", ptr);

    if (is_entry_free(curr)) {
        dprintk("invalid free pointer: %p\r\n", ptr);
        _memerr = -1;
        return;
    }
    set_next_entry(curr, man->head);
    set_entry_free(curr, TRUE);
    man->head  = curr;
}
