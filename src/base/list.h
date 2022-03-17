#ifndef _LIST_HEADER_
#define _LIST_HEADER_

typedef struct tag_dlist_entry {
    struct tag_dlist_entry* prev;
    struct tag_dlist_entry* next;
} dlist_entry_t;

typedef struct tag_dlist {
    dlist_entry_t* head;
    dlist_entry_t* tail;
} dlist_t;

typedef struct tag_slist_entry {
    struct tag_slist_entry* prev;
} slist_entry_t;

typedef struct tag_slist {
    slist_entry_t* head;
    slist_entry_t* tail;
} slist_t;

dlist_t* dlist_create(void);
void dlist_destroy(dlist_t* dlist);
void dlist_append(dlist_t* list, dlist_entry_t* entry);
void dlist_remove(dlist_entry_t* entry);
void dlist_insert(dlist_entry_t* prev, dlist_entry_t* entry);

void slist_create(void);
void slist_append(slist_t* list, slist_entry_t* entry);
void slist_remove(slist_entry_t* entry);
void slist_insert(slist_entry_t* prev, slist_entry_t* entry);

#endif