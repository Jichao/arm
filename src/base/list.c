#include "list.h"
#include "mem/kmalloc.h"

dlist_t* dlist_create(void)
{
    dlist_t* dlist = (dlist_t*)kzmalloc(sizeof(dlist_t));
    return dlist;
}

void dlist_destroy(dlist_t* dlist)
{
    kfree(dlist);
}

void dlist_append(dlist_t* dlist, dlist_entry_t* entry)
{
    if (!dlist->tail) {
        kassert(!dlist->head, "no tail should empty\r\n");
        dlist->head = entry;
        dlist->tail = entry;
        return;
    }
    kassert(dlist->tail->next == NULL, "");
    dlist->tail = entry;
    entry->prev = dlist->tail;

}
void dlist_remove(dlist_entry_t* entry);
void dlist_insert(dlist_entry_t* prev, dlist_entry_t* entry);

void slist_create(void);
void slist_append(slist_t* list, slist_entry_t* entry);
void slist_remove(slist_entry_t* entry);
void slist_insert(slist_entry_t* prev, slist_entry_t* entry);
