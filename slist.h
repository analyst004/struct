/*
	单向链表
	1.0 kimzhang 创建 
 */ 
#ifndef LIB_SLIST_H_
#define LIB_SLIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ANYSIZE
#define ANYSIZE 	1
#endif

#ifndef _ERRCODE_DEFINED
#define _ERRCODE_DEFINED
typedef int errno_t;
#endif

typedef struct _snode_t {
	struct _snode_t * next; //指向下一个节点
	uint8_t data[ANYSIZE];
}snode_t; 

typedef struct _slist_t {
	struct _snode_t * first;//指向第一个节点
	struct _snode_t * last; //指向最后一个节点
	struct _snode_t * current;
	int32_t count;	
}slist_t;

errno_t SListInit(slist_t* slist);

errno_t SListAppend(slist_t * slist, uint8_t* data, size_t data_size);

uint8_t* SListNext(slist_t* slist);

void SListClear(slist_t* slist);

#ifdef __cplusplus
}
#endif

#endif