#pragma once

#include "interpreter/refcount.h"
#include "data-structures/YASL_bytebuffer.h"
#include "interpreter/userdata.h"

#define LS_BASESIZE 4
#define FOR_LIST(i, name, list) struct YASL_Object name; for (size_t i = 0; i < (list)->count && (name = (list)->items[i], 1); i++)

struct YASL_List {
    size_t size;
    size_t count;
    struct YASL_Object *items;
};

int isvalueinarray(int64_t val, int64_t *arr, int size);
struct RC_UserData *ls_new(void);
struct YASL_List *list_new_sized(const size_t base_size);
struct RC_UserData* ls_new_sized(const size_t base_size);
void ls_del_data(void *ls);
void ls_insert(struct YASL_List *const ls, const int64_t index, struct YASL_Object value);
void ls_append(struct YASL_List* ls, struct YASL_Object value);
struct YASL_Object ls_search(struct YASL_List *ls, int64_t index);
void ls_reverse(struct YASL_List *ls);