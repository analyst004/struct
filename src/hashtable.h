/*
	1.0	����
	2.0	�����˽ӿڣ� ʹ��ö�ٽڵ��ʱ�����ɾ���ڵ�
  	2.1 �����˹ر�Zipper��ʱ���ڴ�й¶������
  	3.0 �����˽ڵ㸸�ӹ�ϵ������
  	3.1 ������������ʹ��C99�淶
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

typedef struct _listentry_t {
	struct _listentry_t *Flink;
	struct _listentry_t *Blink;
}listentry_t;

//�����Ľṹ
typedef struct _hashtable_t
{
	int32_t		 NumberOfBurkets;	//�����ĳ���
	int32_t		 NumberOfNode;		//�ڵ�����
	listentry_t	 FastList;		//�ڵ����ö������
	listentry_t*	 HashTable;		//����
}hashtable_t;

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

//����HASH��
errno_t CreateHashTable(
	uint32_t table_length,  
	hashtable_t** hash_table);

//����ڵ�
errno_t InsertHashEntry(
	hashtable_t* table, 
	uint32_t idNode, 
	uint8_t* NodeData, 
	size_t DataSize );

//�������в��ҽڵ�
errno_t  FindHashEntry(
	hashtable_t* table, 
	uint32_t idNode,
	uint8_t** data,
	uint32_t* data_size );

//���ڵ���������Ƴ��� ��û�����٣�
errno_t RemoveHashEntry( 
	hashtable_t* table, 
	uint32_t idNode );

//ö�������е�ÿ���ڵ�
errno_t EnumHashEntry(
	hashtable_t* table, 
	ENUM_ZIPNODE_ROUTINE EnumRoutine, 
	void* Context );

//������еĽڵ�
errno_t	ClearHashTable( 
	hashtable_t* table );

//��ȡ�ڵ�����
uint32_t GetHashEntryCount( 
	hashtable_t* table );

//Ϊ�����ڵ㽨�����ӹ�ϵ
errno_t AddChildHashEntry(
    hashtable_t* table,
    uint32_t idParentNode,
    uint32_t idChildNode);

//��������ڵ�֮��ĸ��ӹ�ϵ
errno_t RemoveChildHashEntry(
    hashtable_t* table,
    uint32_t idParentNode,
    uint32_t idChildNode);

//��ȡָ���ڵ����ӽڵ������
int GetChildHashEntryCount(
	hashtable_t* table, 
	uint32_t idNode);

//�ر������ṹ
void	CloseHashTable(
	hashtable_t* table );

#ifdef __cplusplus
}
#endif

#endif