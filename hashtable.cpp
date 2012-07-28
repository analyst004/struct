#ifdef __KERNEL__
#include <ntddk.h>
#else
#include <windows.h>
#endif
//#include "../base/assert.h"
#include <errno.h>

#include "hashtable.h"

#define	TAG			'ZPER'

#ifdef	__KERNEL__
#define	MallocMemory(size)	ExAllocatePoolWithTag(NonPagedPool, (size), TAG)
#define	ZeroMemory(p, s)	RtlZeroMemory((p), (s))
#define	FreeMemory(p)		ExFreePoolWithTag((p), TAG )
#define	CopyMemory			RtlCopyMemory
#else
#define	MallocMemory(size)	malloc((size))
#define	ZeroMemory(p, s)	memset((p), 0, (s))
#define	FreeMemory(p)		free((p))
#define	CopyMemory 			memcpy
#endif

//�����ڵ�ṹ
typedef struct _binding_t
{
	LIST_ENTRY	Entry;
	LIST_ENTRY	FastEntry;
	ULONG		idNode;			//�����ڵ��
	ULONG		DataSize;		//�ڵ�����
	UCHAR		Data[1];			//�ڵ�����
}binding_t;

//��������
errno_t CreateHashTable(size_t table_length,  hash_table_t** table);
{
	size_t table_size = sizeof(hash_table_t) + sizeof(LIST_ENTRY)*table_length
	hash_table_t* table = (hash_table_t*)MallocMemory(table_size);
	if (table == NULL) {
		return ENOMEM;
	}
	ZeroMemory(table, table_size);
	table->NumberOfBurkets = table_length;
	table->HashTable = (LIST_ENTRY*)((uint8_t*)table + sizeof(hash_table_t))

#ifdef __KERNEL__
	KeInitializeSpinLock( &table->Lock );
#else
	InitializeCriticalSection( &table->Lock );
#endif

	//��ʼ��HASHTABLE
	int32_t	 i =0;
	for( i = 0; i < table->NumberOfBurkets; i++ ) {
		LIST_ENTRY* ListHead =  &table->HashTable[i];
		ListHead->Flink = ListHead->Blink = ListHead;
	}
	
	table->FastList.Flink = table->FastList.Blink = &table->FastList;
	return table;
}

errno_t InsertHashEntry(
	hash_table_t* table, 
	uint32_t idNode, 
	uint8_t* NodeData, 
	size_t DataSize );
{
	if( table == NULL ) {
		return EINVAL;
	}

	//��ȡҳĿ¼��
	binding_t* pPDE  = NULL;
	binding_t* pNode = NULL;
	LIST_ENTRY* pFastList  = NULL;
	int32_t  iBurket = -1; 
	LIST_ENTRY* pListHead = NULL; 

#ifdef __KERNEL__
	KIRQL			irql = 0;
#endif

	iBurket = idNode % table->NumberOfBurkets;
	pListHead = &table->HashTable[iBurket];

	pNode->idNode = idNode;
	pNode->DataSize = DataSize;
	if (DataSize > 0 ) {
		pNode = MallocMemory(DataSize + sizeof(binding_t));
		if (pNode == NULL) {
			return ENOMEM;
		}
		ZeroMemory(pNode, DataSize + sizeof(binding_t));
		CopyMemory(pNode->Data, NodeData, DataSize);
	}
#ifdef __KERNEL__
	KeAcquireSpinLock( &table->Lock, &irql );
#else
	EnterCriticalSection( &table->Lock );
#endif
	
	pPDE = (binding_t*)pListHead->Flink;
	while( (LIST_ENTRY*)pPDE != pListHead ) {
		if( pPDE->idNode == pNode->idNode ) {
			break;
		} 

		//��һ��
		pPDE = (ZIPPER_NODE*)pPDE->Entry.Flink;
	}

	if( (LIST_ENTRY*)pPDE != pListHead ) {
		//�Ѿ�����
#ifdef __KERNEL__
		KeReleaseSpinLock( &table->Lock, irql );
#else
		LeaveCriticalSection( &table->Lock );
#endif
		return false;
	}

	//�����ڣ� ���Բ���
	pNode->Entry.Flink = pListHead->Flink;
	pNode->Entry.Flink->Blink = (LIST_ENTRY*)pNode;
	pListHead->Flink = (LIST_ENTRY*)pNode;
	pNode->Entry.Blink = pListHead;

	//ͬʱ��Ҫ���뵽FAST�б���, �Ա��ڿ���ö�ٽڵ�
	
	pFastList = &table->FastList;
	pNode->FastEntry.Flink = pFastList->Flink;
	pNode->FastEntry.Blink = pFastList;
	pFastList->Flink->Blink = &pNode->FastEntry;
	pFastList->Flink = &pNode->FastEntry;
	
	table->NumberOfNode++;

#ifdef __KERNEL__
	KeReleaseSpinLock( &table->Lock, irql );
#else
	LeaveCriticalSection( &table->Lock );
#endif
	return true;
}

errno_t RemoveHashEntry( 
	hash_table_t* table, 
	uint32_t idNode )
{
	//��ȡҳĿ¼��
	binding_t* pZipNode  = NULL;
	uint32_t  iBurket = idNode % pZipper->NumberOfBurkets;
	LIST_ENTRY* pListHead = &pZipper->HashTable[iBurket];
#ifdef __KERNEL__
	KIRQL			irql = 0;
#endif

	if (table == NULL) {
		return EINVAL;
	}

#ifdef __KERNEL__
	KeAcquireSpinLock( &table->Lock, &irql );
#else
	EnterCriticalSection( &table->Lock );
#endif

	pZipNode = (binding_t*)pListHead->Flink;
	while( (LIST_ENTRY*)pZipNode != pListHead ) {
		if( pZipNode->idNode == idNode ) {
			break;
		} 
		//��һ��
		pZipNode = (binding_t*)pZipNode->Entry.Flink;
	}

	if( (LIST_ENTRY*)pZipNode == pListHead ) {
		//������
#ifdef __KERNEL__
		KeReleaseSpinLock( &table->Lock, irql );
#else
		LeaveCriticalSection( &table->Lock );
#endif
		return ENOENT;
	}

	//���ڵ��ҳĿ¼��ɾ��
	pZipNode->Entry.Blink->Flink = pZipNode->Entry.Flink;
	pZipNode->Entry.Flink->Blink = pZipNode->Entry.Blink;
	pZipNode->Entry.Blink = (LIST_ENTRY*)pZipNode;
	pZipNode->Entry.Flink = (LIST_ENTRY*)pZipNode;

	//���ڵ��FASTö���б���ɾ��
	pZipNode->FastEntry.Blink->Flink = pZipNode->FastEntry.Flink;
	pZipNode->FastEntry.Flink->Blink = pZipNode->FastEntry.Blink;
	pZipNode->FastEntry.Flink = &pZipNode->FastEntry;
	pZipNode->FastEntry.Blink = &pZipNode->FastEntry;

	//�ͷŽڵ�����
	FreeMemory( pZipNode );
	pZipNode = NULL;

	table->NumberOfNode--;

	if( table->NumberOfNode == 0 ) {
		ZASSERT( table->FastList.Blink == &table->FastList );
		ZASSERT( table->FastList.Flink == &table->FastList );
		ZASSERT(  pListHead->Blink == pListHead );
		ZASSERT( pListHead->Flink == pListHead );
	}

#ifdef __KERNEL__
	KeReleaseSpinLock( &table->Lock, irql );
#else
	LeaveCriticalSection( &table->Lock );
#endif
	return 0;
}

errno_t  FindHashEntry(
	hash_table_t* table, 
	uint32_t idNode,
	uint8_t** data,
	uint32_t* data_size )
{
	if (table == NULL) {
		return EINVAL;
	}
	//��ȡҳĿ¼��
	binding_t* pPDE  = NULL;
	uint32_t  iBurket = idNode % table->NumberOfBurkets;
	LIST_ENTRY* pListHead = &table->HashTable[iBurket];

#ifdef __KERNEL__
	KIRQL			irql = 0;
	KeAcquireSpinLock( &table->Lock, &irql );
#else
	EnterCriticalSection( &table->Lock );
#endif

	pPDE = (binding_t*)pListHead->Flink;
	while( (LIST_ENTRY*)pPDE != pListHead ) {
		if( pPDE->idNode == idNode ) {
			break;
		} 
		//��һ��
		pPDE = (binding_t*)pPDE->Entry.Flink;
	}

#ifdef __KERNEL__
	KeReleaseSpinLock( &table->Lock, irql );
#else
	LeaveCriticalSection( &table->Lock );
#endif

	if( (LIST_ENTRY*)pPDE == pListHead ) {
		//û���ҵ�
		return ENOENT;
	}

	//�ҵ�
	*data = pPDE->Data;
	*data_size = pPDE->DataSize;
	return 0;
}

errno_t EnumHashEntry(
	hash_table_t* table, 
	ENUM_ZIPNODE_ROUTINE EnumRoutine, 
	PVOID Context )
{
	if (table == NULL) {
		return EINVAL;
	}

	if (EnumRoutine == NULL) {
		return EINVAL;
	}

	INT		NextAction = ENUM_CONTINUE;
#ifdef __KERNEL__
	KIRQL			irql = 0;
#endif	
	LIST_ENTRY* pListHead = NULL;
	LIST_ENTRY* pNode = NULL;

	//�Ƚ�������
#ifdef __KERNEL__
	KeAcquireSpinLock( &table->Lock, &irql );
#else
	EnterCriticalSection( &table->Lock );
#endif	

	pListHead = &table->FastList;
	pNode = pListHead->Flink;
	while( (LIST_ENTRY*)pNode != pListHead ) {
		
		binding_t* pZipNode = (binding_t*)((uint8_t*)pNode - sizeof(LIST_ENTRY));
		NextAction = EnumRoutine( pZipNode->idNode, pZipNode->Data, 
					  pZipNode->DataSize, pContext );
		pNode = pNode->Flink;
		if (NextAction & ENUM_DELETE) {
			//��Ҫɾ���ýڵ�
			//���ڵ���б���ɾ��
			pZipNode->Entry.Blink->Flink = pZipNode->Entry.Flink;
			pZipNode->Entry.Flink->Blink = pZipNode->Entry.Blink;
			pZipNode->Entry.Blink = (LIST_ENTRY*)pZipNode;
			pZipNode->Entry.Flink = (LIST_ENTRY*)pZipNode;

			//���ڵ��FASTö���б���ɾ��
			pZipNode->FastEntry.Blink->Flink = pZipNode->FastEntry.Flink;
			pZipNode->FastEntry.Flink->Blink = pZipNode->FastEntry.Blink;
			pZipNode->FastEntry.Flink = &pZipNode->FastEntry;
			pZipNode->FastEntry.Blink = &pZipNode->FastEntry;

			table->NumberOfNode--;
			FreeMemory( pZipNode );
			table = NULL;
		} 

		if( NextAction & ENUM_CONTINUE ) {
			//����
		} else {
			//ֹͣö��
			break;
		}
	}

#ifdef __KERNEL__
	KeReleaseSpinLock( &table->Lock, irql );
#else
	LeaveCriticalSection( &table->Lock );
#endif

	return NextAction & ENUM_CONTINUE;
}

errno_t	ClearHashTable( 
	hash_table_t* table )
{
	if( table == NULL ) {
		return EINVAL;
	}

#ifdef __KERNEL__
	KIRQL			irql = 0;
#endif	
	LIST_ENTRY* pListHead = NULL;
	LIST_ENTRY* pNode = NULL;

#ifdef __KERNEL__
	KeAcquireSpinLock( &pZipper->Lock, &irql );
#else
	EnterCriticalSection( &pZipper->Lock );
#endif	

	pListHead = &pZipper->FastList;
	pNode = pListHead->Flink;
	while( pNode != pListHead ) {

		//���ڵ���������Ƴ�
		binding_t* pZipNode = (binding_t*)((UCHAR*)pNode - sizeof(LIST_ENTRY));

		pNode =pNode->Flink;

		//���ڵ��ҳĿ¼��ɾ��
		pZipNode->Entry.Blink->Flink = pZipNode->Entry.Flink;
		pZipNode->Entry.Flink->Blink = pZipNode->Entry.Blink;
		pZipNode->Entry.Blink = (LIST_ENTRY*)pZipNode;
		pZipNode->Entry.Flink = (LIST_ENTRY*)pZipNode;

		//���ڵ��FASTö���б���ɾ��
		pZipNode->FastEntry.Blink->Flink = pZipNode->FastEntry.Flink;
		pZipNode->FastEntry.Flink->Blink = pZipNode->FastEntry.Blink;
		pZipNode->FastEntry.Flink = &pZipNode->FastEntry;
		pZipNode->FastEntry.Blink = &pZipNode->FastEntry;

		table->NumberOfNode--;

		FreeMemory( pZipNode );
		pZipNode = NULL;
	
	/*	if( ClearRoutine ) {
			bContinue = ClearRoutine( pZipNode->Data, Context );
		} else {
			bContinue = TRUE;
		}
		
		if( !bContinue )
			break;*/
	}

	//if( bContinue ) {
	//	ZASSERT( table->NumberOfNode == 0 );
	//	ZASSERT( table->FastList.Flink == &table->FastList );
	//	ZASSERT( table->FastList.Blink == &table->FastList );
	//	ZASSERT( pListHead->Blink == pListHead );
	//	ZASSERT( pListHead->Flink == pListHead );
	//}


#ifdef __KERNEL__
	KeReleaseSpinLock( &table->Lock, irql );
#else
	LeaveCriticalSection( &table->Lock );
#endif
	
	return 0;
}

ULONG HashTableGetNodeCount( hash_table_t* pZipper )
{
	ULONG nr = 0;
#ifdef __KERNEL__
	KIRQL			irql = 0;
#endif	

	if( pZipper == NULL )
		return 0;

#ifdef __KERNEL__
	KeAcquireSpinLock( &pZipper->Lock, &irql );
#else
	EnterCriticalSection( &pZipper->Lock );
#endif	

	nr = pZipper->NumberOfNode;

#ifdef __KERNEL__
	KeReleaseSpinLock( &pZipper->Lock, irql );
#else
	LeaveCriticalSection( &pZipper->Lock );
#endif
	return nr;

}

void HashTableClose( hash_table_t* pZipper )
{
	if( pZipper == NULL )
		return;

	ZASSERT( HashTableClear( pZipper ) );
	
	if( pZipper->HashTable ) {
		FreeMemory( pZipper->HashTable );
		pZipper->HashTable = NULL;
	}

	FreeMemory( pZipper );
}
