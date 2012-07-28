/*
	1.0	创建
	2.0	更改了接口， 使得枚举节点的时候可以删除节点
		修正了关闭Zipper的时候内存泄露的问题
 */
#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifndef _ERRCODE_DEFINED
#define _ERRCODE_DEFINED
typedef int errno_t;
#endif

//拉链的结构
typedef struct _hash_table_t
{
	int32_t		 NumberOfBurkets;	//拉链的长度
	int32_t		 NumberOfNode;		//节点总数
	LIST_ENTRY	 FastList;		//节点快速枚举链表
#ifdef __KERNEL__
	KSPIN_LOCK	 Lock;			//同步锁
#else
	CRITICAL_SECTION Lock;				
#endif
	LIST_ENTRY*	 HashTable;		//拉链
}hash_table_t;

#define ENUM_STOP	0
#define	ENUM_CONTINUE	1
#define ENUM_DELETE	2

/*
	拉链节点枚举回调函数
	返回值:		ENUM_STOP		继续枚举
			ENUM_CONTINUE		停止枚举
			ENUM_DELETE		删除该节点
	备注:	在枚举回调函数中不能释放缓冲区
*/
typedef errno_t (*ENUM_ZIPNODE_ROUTINE)( 
	uint32_t idNode, 
	uint8_t* pNodeData, 
	size_t	 DataSize,
	void* Context );

//创建拉链
hash_table_t* CreateHashTable( size_t table_length );

//插入节点
errno_t InsertHashEntry(
	hash_table_t* table, 
	uint32_t idNode, 
	uint8_t* NodeData, 
	size_t DataSize );

//在拉链中查找节点
void*   FindHashEntry(
	hash_table_t* table, 
	uint32_t idNode );

//将节点从拉链中移除（ 并没有销毁）
errno_t RemoveHashEntry( 
	hash_table_t* table, 
	uint32_t idNode );

//枚举拉链中的每个节点
errno_t EnumHashEntry(
	hash_table_t* table, 
	ENUM_ZIPNODE_ROUTINE EnumRoutine, 
	PVOID Context );

//清除所有的节点
errno_t	ClearHashTable( 
	hash_table_t* table );

//获取节点总数
uint32_t GetHashEntryCount( 
	hash_table_t* table );

//关闭拉链结构
void	CloseHashTable(
	hash_table_t* table );

#ifdef __cplusplus
}
#endif

#endif