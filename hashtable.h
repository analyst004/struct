/*
	1.0	����
	2.0	�����˽ӿڣ� ʹ��ö�ٽڵ��ʱ�����ɾ���ڵ�
		�����˹ر�Zipper��ʱ���ڴ�й¶������
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

//�����Ľṹ
typedef struct _hash_table_t
{
	int32_t		 NumberOfBurkets;	//�����ĳ���
	int32_t		 NumberOfNode;		//�ڵ�����
	LIST_ENTRY	 FastList;		//�ڵ����ö������
#ifdef __KERNEL__
	KSPIN_LOCK	 Lock;			//ͬ����
#else
	CRITICAL_SECTION Lock;				
#endif
	LIST_ENTRY*	 HashTable;		//����
}hash_table_t;

#define ENUM_STOP	0
#define	ENUM_CONTINUE	1
#define ENUM_DELETE	2

/*
	�����ڵ�ö�ٻص�����
	����ֵ:		ENUM_STOP		����ö��
			ENUM_CONTINUE		ֹͣö��
			ENUM_DELETE		ɾ���ýڵ�
	��ע:	��ö�ٻص������в����ͷŻ�����
*/
typedef errno_t (*ENUM_ZIPNODE_ROUTINE)( 
	uint32_t idNode, 
	uint8_t* pNodeData, 
	size_t	 DataSize,
	void* Context );

//��������
hash_table_t* CreateHashTable( size_t table_length );

//����ڵ�
errno_t InsertHashEntry(
	hash_table_t* table, 
	uint32_t idNode, 
	uint8_t* NodeData, 
	size_t DataSize );

//�������в��ҽڵ�
void*   FindHashEntry(
	hash_table_t* table, 
	uint32_t idNode );

//���ڵ���������Ƴ��� ��û�����٣�
errno_t RemoveHashEntry( 
	hash_table_t* table, 
	uint32_t idNode );

//ö�������е�ÿ���ڵ�
errno_t EnumHashEntry(
	hash_table_t* table, 
	ENUM_ZIPNODE_ROUTINE EnumRoutine, 
	PVOID Context );

//������еĽڵ�
errno_t	ClearHashTable( 
	hash_table_t* table );

//��ȡ�ڵ�����
uint32_t GetHashEntryCount( 
	hash_table_t* table );

//�ر������ṹ
void	CloseHashTable(
	hash_table_t* table );

#ifdef __cplusplus
}
#endif

#endif