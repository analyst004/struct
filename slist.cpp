#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#ifdef __KERNEL__
#include <ntddk.h>
#else
#include <windows.h>
#endif
#include "slist.h"


errno_t SListInit(slist_t* slist)
{
	if (slist == NULL)
		return EINVAL;

	memset(slist, 0, sizeof(slist_t));
	return 0;
}

errno_t SListAppend(slist_t * slist, uint8_t* data, size_t data_size)
{
	if (slist == NULL || data == NULL || data_size == 0) {
		return EINVAL;
	}

	snode_t* node = (snode_t*)malloc(sizeof(void*) + data_size);
	if (node == NULL) {
	  return ENOMEM;
	}
	memset(node , 0, sizeof(snode_t));

	memcpy(node->data, data, data_size);

	if (slist->last != NULL ) {
		slist->last->next = node;
	}
	
	slist->last = node;
	if (slist->first == NULL) {
		slist->first = node;
	}

	if (slist->current == NULL) {
		slist->current = node;
	}
	slist->count ++;

	return 0;
}

uint8_t* SListNext(slist_t* slist)
{
	if (slist == NULL) {
		return NULL;
	}

	uint8_t* data = NULL;	

	if (slist->current == NULL) {
		data = NULL;
	} else {
		data = slist->current->data;
		slist->current = slist->current->next;		
	}

	return data;
}

void SListClear(slist_t* slist)
{
	if (slist == NULL || slist->first == NULL || slist->last == NULL)
		return;
	snode_t* current = slist->first;
	while(current != NULL) {
		snode_t* delete_node = current;
		current = current->next;
		free(delete_node);
		delete_node = NULL;
	}
	memset(slist, 0, sizeof(slist_t));
} 