#ifndef _STACK_H_KIMZHANG
#define _STACK_H_KIMZHANG

#include "slist.h"

#define stack_t 	slist_t
#define stack_init  slist_init
#define stack_push	slist_add
#define stack_pop	slist_remove_head
#define stack_entry	slist_entry

#endif //_STACK_H_KIMZHANG