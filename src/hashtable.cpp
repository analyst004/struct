#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include "hashtable.h"

#define 	TAG                     'ZPER'

#ifdef    __KERNEL__
#define   _MallocMemory( size ) \
  ExAllocatePoolWithTag( NonPagedPool, (size), TAG_ZIPPER )
#define   _ZeroMemory( p, s )  		RtlZeroMemory( (p), (s) )
#define   _FreeMemory( p )     		ExFreePoolWithTag( (p), TAG_ZIPPER )
#define   _CopyMemory    			RtlCopyMemory
#else
#define   _MallocMemory( size )     malloc( (size ) )
#define   _ZeroMemory( p, s )  		memset( (p), 0,  (s) )
#define   _FreeMemory( p )          free( (p) )
#define   _CopyMemory            	memcpy
#endif

//拉链节点结构
typedef struct _hashentry_t
{
  listentry_t  			Entry;
  listentry_t  			FastEntry;
  listentry_t  			BrotherEntry;
  struct _hashentry_t*	Child;
  struct _hashentry_t*	Parent;
  uint32_t   			idNode;     //拉链节点号
  uint32_t   			DataSize;   //节点数据
  uint8_t   			Data[1];      //节点数据
}hashentry_t;

//创建拉链
errno_t CreateHashTable( uint32_t length, hashtable_t** hashtable )
{
  if (hashtable == NULL) {
    return EINVAL;
  }
  hashtable_t* table = (hashtable_t*)_MallocMemory(sizeof(hashtable_t));
  if( table == NULL ) {
    return ENOMEM;
  }
  _ZeroMemory(table, sizeof(hashtable_t));

  table->NumberOfBurkets = length;
  table->HashTable = (listentry_t*)_MallocMemory(
  	sizeof(listentry_t) * table->NumberOfBurkets);
  _ZeroMemory( table->HashTable, sizeof(listentry_t) * table->NumberOfBurkets );

  //初始化HASHTABLE
  for( uint32_t i = 0; i < table->NumberOfBurkets; i++ ) {
    listentry_t* ListHead =  &table->HashTable[i];
    ListHead->Flink = ListHead->Blink = ListHead;
  }

  table->FastList.Flink = table->FastList.Blink = &table->FastList;
  *hashtable = table;
  return 0;
}

errno_t InsertHashEntry(
	hashtable_t* table, 
	uint32_t idNode, 
	uint8_t* NodeData,
  uint32_t  DataSize )
{
  //获取页目录项
  hashentry_t* pPDE  = NULL;
  hashentry_t* pNode = NULL;
  listentry_t* pFastList  = NULL;
  uint32_t	  iBurket = -1;
  listentry_t* pListHead = NULL;

  if( table == NULL ) {
    return EINVAL;
  }

  iBurket = idNode % table->NumberOfBurkets;
  pListHead = &table->HashTable[iBurket];
  pNode = (hashentry_t*)_MallocMemory( DataSize + sizeof( hashentry_t ) );
  if (pNode == NULL) {
  	return ENOMEM;
  }
  _ZeroMemory( pNode, DataSize + sizeof( hashentry_t ) );
  pNode->idNode = idNode;
  pNode->DataSize = DataSize;
  _CopyMemory( pNode->Data, NodeData, DataSize );

  pPDE = (hashentry_t*)pListHead->Flink;

  while( (listentry_t*)pPDE != pListHead ) {
    if( pPDE->idNode == pNode->idNode ) {
      break;
    }

    //下一个
    pPDE = (hashentry_t*)pPDE->Entry.Flink;
  }

  if( (listentry_t*)pPDE != pListHead ) {
    //已经存在
    return EEXIST;
  }

  //不存在， 可以插入
  pNode->Entry.Flink = pListHead->Flink;
  pNode->Entry.Flink->Blink = (listentry_t*)pNode;
  pListHead->Flink = (listentry_t*)pNode;
  pNode->Entry.Blink = pListHead;

  //同时还要插入到FAST列表中, 以便于快速枚举节点
  pFastList = &table->FastList;
  pNode->FastEntry.Flink = pFastList->Flink;
  pNode->FastEntry.Blink = pFastList;
  pFastList->Flink->Blink = &pNode->FastEntry;
  pFastList->Flink = &pNode->FastEntry;

  //初始化兄弟节点
  pNode->BrotherEntry.Flink = pNode->BrotherEntry.Blink = &pNode->BrotherEntry;

  pNode->Child = NULL;
  pNode->Parent = NULL;

  table->NumberOfNode++;
  return 0;
}

errno_t RemoveHashEntry(hashtable_t* table, uint32_t idNode )
{
  if (table == NULL)
  	return EINVAL;

  //获取节点
  hashentry_t* entry  = NULL;
  uint32_t  iBurket = idNode % table->NumberOfBurkets;
  listentry_t* pListHead = &table->HashTable[iBurket];

  entry = (hashentry_t*)pListHead->Flink;
  while( (listentry_t*)entry != pListHead ) {
    if( entry->idNode == idNode ) {
      break;
    }
    //下一个
    entry = (hashentry_t*)entry->Entry.Flink;
  }

  if( (listentry_t*)entry == pListHead ) {
    //不存在
    return ENOENT;
  }

  //将节点从页目录中删除
  entry->Entry.Blink->Flink = entry->Entry.Flink;
  entry->Entry.Flink->Blink = entry->Entry.Blink;
  entry->Entry.Blink = (listentry_t*)entry;
  entry->Entry.Flink = (listentry_t*)entry;

  //将节点从FAST枚举列表中删除
  entry->FastEntry.Blink->Flink = entry->FastEntry.Flink;
  entry->FastEntry.Flink->Blink = entry->FastEntry.Blink;
  entry->FastEntry.Flink = &entry->FastEntry;
  entry->FastEntry.Blink = &entry->FastEntry;

  //释放节点数据
  _FreeMemory( entry );
  entry = NULL;

  table->NumberOfNode--;

  /*
  if( table->NumberOfNode == 0 ) {
    ZASSERT( table->FastList.Blink == &table->FastList );
    ZASSERT( table->FastList.Flink == &table->FastList );
    ZASSERT(  pListHead->Blink == pListHead );
    ZASSERT( pListHead->Flink == pListHead );
  }
  */
  return 0;
}

errno_t  FindHashEntry(
	hashtable_t* table, 
	uint32_t idNode,
	uint8_t** data,
	uint32_t* data_size )
{
  if (table == NULL)
  	return EINVAL;

  //获取页目录项
  hashentry_t* pPDE  = NULL;
  uint32_t  iBurket = idNode % table->NumberOfBurkets;

  listentry_t* pListHead = &table->HashTable[iBurket];
  pPDE = (hashentry_t*)pListHead->Flink;

  while( (listentry_t*)pPDE != pListHead ) {
    if( pPDE->idNode == idNode ) {
      break;
    }
    //下一个
    pPDE = (hashentry_t*)pPDE->Entry.Flink;
  }

  if( (listentry_t*)pPDE == pListHead ) {
    //没有找到
    return ENOENT;
  }

  //找到
  *data = pPDE->Data;
  *data_size = pPDE->DataSize;
  return 0;
}

errno_t EnumHashEntry(
	hashtable_t* table, 
	ENUM_ZIPNODE_ROUTINE EnumRoutine, 
	void* Context )
{
  if (table == NULL || EnumRoutine == NULL)
  	return EINVAL;

  int   NextAction = ENUM_CONTINUE;
  listentry_t* pListHead = NULL;
  listentry_t* pNode = NULL;

  //先进行锁定

  pListHead = &table->FastList;
  pNode = pListHead->Flink;
  while( (listentry_t*)pNode != pListHead ) {

    hashentry_t* entry = (hashentry_t*)((uint8_t*)pNode - sizeof(listentry_t));
    NextAction = EnumRoutine( entry->idNode, entry->Data,
            entry->DataSize, Context );
    pNode = pNode->Flink;
    if( NextAction & ENUM_DELETE ) {
      //需要删除该节点
      //将节点从列表中删除
      entry->Entry.Blink->Flink = entry->Entry.Flink;
      entry->Entry.Flink->Blink = entry->Entry.Blink;
      entry->Entry.Blink = (listentry_t*)entry;
      entry->Entry.Flink = (listentry_t*)entry;

      //将节点从FAST枚举列表中删除
      entry->FastEntry.Blink->Flink = entry->FastEntry.Flink;
      entry->FastEntry.Flink->Blink = entry->FastEntry.Blink;
      entry->FastEntry.Flink = &entry->FastEntry;
      entry->FastEntry.Blink = &entry->FastEntry;

      table->NumberOfNode--;
      _FreeMemory( entry );
      entry = NULL;
    }

    if( NextAction & ENUM_CONTINUE ) {
      //继续
    } else {
      //停止枚举
      break;
    }
  }
  return NextAction & ENUM_CONTINUE;
}

errno_t	ClearHashTable(hashtable_t* table)
{
  if( table == NULL ) {
    return EINVAL;
  }

  listentry_t* pListHead = NULL;
  listentry_t* pNode = NULL;

  pListHead = &table->FastList;
  pNode = pListHead->Flink;
  while( pNode != pListHead ) {

    //将节点从拉链中移除
    hashentry_t* entry = (hashentry_t*)((uint8_t*)pNode- sizeof( listentry_t) );

    pNode =pNode->Flink;

    //将节点从页目录中删除
    entry->Entry.Blink->Flink = entry->Entry.Flink;
    entry->Entry.Flink->Blink = entry->Entry.Blink;
    entry->Entry.Blink = (listentry_t*)entry;
    entry->Entry.Flink = (listentry_t*)entry;

    //将节点从FAST枚举列表中删除
    entry->FastEntry.Blink->Flink = entry->FastEntry.Flink;
    entry->FastEntry.Flink->Blink = entry->FastEntry.Blink;
    entry->FastEntry.Flink = &entry->FastEntry;
    entry->FastEntry.Blink = &entry->FastEntry;

    table->NumberOfNode--;

    _FreeMemory( entry );
    entry = NULL;

  /*  if( ClearRoutine ) {
      bContinue = ClearRoutine( entry->Data, Context );
    } else {
      bContinue = TRUE;
    }

    if( !bContinue )
      break;*/
  }

  /*
  ZASSERT( table->NumberOfNode == 0 );
  ZASSERT( table->FastList.Flink == &table->FastList );
  ZASSERT( table->FastList.Blink == &table->FastList );
  ZASSERT( pListHead->Blink == pListHead );
  ZASSERT( pListHead->Flink == pListHead );
  */

  return 0;
}

uint32_t GetHashEntryCount(hashtable_t* table)
{
  uint32_t nr = 0;

  if( table == NULL )
    return EINVAL;

  nr = table->NumberOfNode;
  return nr;
}

hashentry_t *_GetZipperNode(hashtable_t* table, uint32_t idNode)
{
  hashentry_t *entry  = NULL;
  hashentry_t *pPDE = NULL;
  uint32_t  iBurket = idNode % table->NumberOfBurkets;
  listentry_t* pListHead = &table->HashTable[iBurket];

  pPDE = (hashentry_t*)pListHead->Flink;

  while( (listentry_t*)pPDE != pListHead ) {
    if( pPDE->idNode == idNode ) {
      break;
    }
    //下一个
    pPDE = (hashentry_t*)pPDE->Entry.Flink;
  }

  if( (listentry_t*)pPDE == pListHead ) {
    //没有找到
    return NULL;
  }

  return pPDE;
}


errno_t AddChildHashEntry(
    hashtable_t* table,
    uint32_t idParentNode,
    uint32_t idChildNode)
{
  errno_t     ret = 0;
  hashentry_t *pParentNode = NULL;
  hashentry_t *pChildNode = NULL;
  listentry_t *ChildList  = NULL;

  if( table == NULL )
    return EINVAL;

  pParentNode = _GetZipperNode(table, idParentNode);
  pChildNode = _GetZipperNode(table, idChildNode);
  if (pParentNode == NULL || pChildNode == NULL) {
    goto ret_door;
  }

  //加入兄弟队列
  if (pParentNode->Child == NULL) {
    //没有子节点
    ChildList = &pChildNode->BrotherEntry;
  } else {
    //有子节点了
    ChildList = &pParentNode->Child->BrotherEntry;
  }
  pChildNode->BrotherEntry.Flink = ChildList->Flink;
  pChildNode->BrotherEntry.Blink = ChildList;
  ChildList->Flink->Blink = &pChildNode->BrotherEntry;
  ChildList->Flink = &pChildNode->BrotherEntry;

  //成为父节点最新的子字节
  pParentNode->Child = pChildNode;
  pChildNode->Parent = pParentNode;

  ret = 0;

ret_door:
  return ret;
}

errno_t RemoveChildHashEntry(
    hashtable_t* table,
    uint32_t idParentNode,
    uint32_t idChildNode)
{
  if( table == NULL )
    return EINVAL;

  errno_t ret = 0;
  hashentry_t *pParentNode = NULL;
  hashentry_t *pChildNode = NULL;

  pParentNode = _GetZipperNode(table, idParentNode);
  pChildNode = _GetZipperNode(table, idChildNode);
  if (pParentNode == NULL || pChildNode == NULL) {
    goto ret_door;
  }

  //不再是父节点最新的子节点
  if (pParentNode->Child == pChildNode) {
    if (pChildNode->BrotherEntry.Flink == &pChildNode->BrotherEntry) {
      //父节点只有一个儿子
      pParentNode->Child = NULL;
    } else {
      pParentNode->Child = (hashentry_t*)pChildNode->BrotherEntry.Flink;
    }
  }
  pChildNode->Parent = NULL;

  //从兄弟节点中删除
  pChildNode->BrotherEntry.Blink->Flink = pChildNode->BrotherEntry.Flink;
  pChildNode->BrotherEntry.Flink->Blink = pChildNode->BrotherEntry.Blink;
  pChildNode->BrotherEntry.Flink = &pChildNode->BrotherEntry;
  pChildNode->BrotherEntry.Blink = &pChildNode->BrotherEntry;

  ret = 0;

ret_door:
  return ret;
}

int GetChildHashEntryCount(hashtable_t* table, uint32_t idNode)
{
  if (table == NULL)
  	return 0;

  int       child_count = 0;
  hashentry_t *entry = NULL;
  listentry_t *ChildList = NULL;
  listentry_t *CurrentNode = NULL;

  entry = _GetZipperNode(table, idNode);
  if (entry == NULL) {
    goto ret_door;
  }

  if (entry->Child == NULL) {
    goto ret_door;
  }

  ChildList = &entry->Child->BrotherEntry;
  CurrentNode = ChildList;
  do {
    child_count++;
    CurrentNode = CurrentNode->Flink;
  } while (CurrentNode != ChildList);

ret_door:
  return child_count;
}

void CloseHashTable( hashtable_t* table )
{
  if( table == NULL )
    return;

  ClearHashTable( table );

  if (table->HashTable) {
    _FreeMemory(table->HashTable);
    table->HashTable = NULL;
  }

  _FreeMemory( table );
}
