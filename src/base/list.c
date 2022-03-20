#include "list.h"
#include "mem/kmalloc.h"

static void dlist_init(dlist_t *dlist, dlist_entry_t *entry)
{
    dlist->head = entry;
    dlist->tail = entry;
    entry->prev = nullptr;
    entry->next = nullptr;
}

dlist_t *dlist_create(dlist_entry_t *entry)
{
    dlist_t *dlist = (dlist_t *)kzmalloc(sizeof(dlist_t));
    dlist_init(dlist, entry);
    return dlist;
}

void dlist_destroy(dlist_t *dlist) { kfree(dlist); }

void dlist_append(dlist_t *dlist, dlist_entry_t *entry)
{
    if (!dlist->tail) {
        kassert(!dlist->head, "no tail should empty\r\n");
        dlist_init(dlist, entry);
    }
    kassert(dlist->tail->next == NULL, "tail next should be nullptr");
    entry->next = nullptr;
    entry->prev = dlist->tail;
    dlist->tail->next = entry;
    dlist->tail = entry;
}

bool dlist_valid(dlist_t *dlist)
{
    if (!dlist->tail) {
        return true;
    }

    if ((dlist->head && !dlist->tail) || (!dlist->head && dlist->tail)) {
        return false;
    }

    if (dlist->head->prev != nullptr || dlist->tail->next != nullptr) {
        return false;
    }

    dlist_entry_t *entry = dlist->head;
    dlist_entry_t *prev = nullptr;
    for (; entry != dlist->tail; entry = entry->next) {
        if (entry->prev != prev) {
            return false;
        }
        prev = entry;
    }
    return true;
}

void dlist_remove(dlist_t* dlist, dlist_entry_t *entry) 
{
    if (entry == dlist->head) {
        dlist->head = dlist->head->next;
        if (dlist->head) {
            dlist->head->prev = nullptr;
        }
        return;
    }

    if (entry == dlist->tail) {
        dlist->tail = dlist->tail->prev;
        if (dlist->tail) {
            dlist->tail->next = nullptr;
        }
        return;
    }

    dlist_entry_t* prev = dlist->head;
    for (dlist_entry_t* dentry = dlist->head->next; dentry != dlist->tail; dentry = dentry->next) {
        if (dentry == entry) {
            prev->next = entry->next;
            entry->next->prev = prev;
            return;
        }
    }
    return;
}

void dlist_insert(dlist_t* dlist, dlist_entry_t *prev, dlist_entry_t *entry) 
{
    if (!dlist->head && !prev) {
        dlist_init(dlist, entry);
        return;
    }

    dlist_entry_t* onext = prev->next;
    prev->next = entry;
    entry->next = onext;
    onext->prev = entry;
    entry->prev = prev;
}

void slist_create(void);
void slist_append(slist_t *list, slist_entry_t *entry);
void slist_remove(slist_entry_t *entry);
void slist_insert(slist_entry_t *prev, slist_entry_t *entry);
