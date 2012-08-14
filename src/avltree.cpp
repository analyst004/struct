#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "avltree.h"
#include "errno.h"

#define MAX(a,b)	((a)>(b)?(a):(b))

int GetHeight(avltree_t* tree)
{
	if (tree == NULL) {
		return -1;
	}
	return tree->height;
}


/*
	左左的情况
	单向右旋平衡处理
 */
avltree_t* LLRotate(avltree_t* tree)
{	
	//树的左节点被选为新的根节点
	avltree_t* root = tree->left;
	root->parent = tree->parent;
	tree->parent = root;
	tree->left= root->right;
	if (tree->left != NULL) {
		tree->left->parent = tree;
	}
	root->right = tree;
	tree->height = MAX(GetHeight(tree->left), GetHeight(tree->right)) + 1;
	root->height = MAX(GetHeight(root->left), tree->height) + 1;
	return root;
}

/* 
	RR旋转 
	单向左旋平衡处理
 */
avltree_t* RRRotate(avltree_t* tree)
{
	avltree_t* root = tree->right;
	root->parent = tree->parent;
	tree->parent = root;
	tree->right = tree->left;
	if (tree->right != NULL) {
		tree->right->parent = tree;
	}
	root->left = tree;
	tree->height = MAX(GetHeight(tree->left), GetHeight(tree->right)) + 1;
	root->height = MAX(tree->height, GetHeight(root->right)) + 1;
	return root;
}

/* 
	LR旋转
	双向旋转(先左后右)平衡处理
 */
avltree_t* LRRotate(avltree_t* tree)
{
	tree = RRRotate(tree);
	return LLRotate(tree);
}

/* 
	RL旋转 
	双向旋转(先右后左)平衡处理
 */
avltree_t* RLRotate(avltree_t* tree)
{
	tree = LLRotate(tree);
	return RRRotate(tree);
}

errno_t InsertAvlEntry(
	avltree_t* tree, 
	uint32_t min_key,
	uint32_t max_key,
	char* data, 
	int data_len, 
	avltree_t** new_root)
{
	if (tree == NULL) {
		tree = (avltree_t*)malloc(sizeof(avltree_t) + data_len);
		if (tree == NULL) {
			return ENOMEM;
		}
		memset(tree, 0, sizeof(avltree_t) + data_len);
		tree->data = (char*)tree + sizeof(avltree_t);
		tree->min_key = min_key;
		tree->max_key = max_key;
		if ( data_len > 0) {
			memcpy(tree->data, data, data_len);
		}
		tree->height = 0;
	} else if ( (min_key >= tree->min_key && min_key <= tree->max_key)
		|| (max_key >= tree->min_key && max_key <= tree->max_key) ) {
		//有重叠部分, 则不进行插入, 也不做任何调整
		*new_root = tree;
		return EEXIST;
	} else if(max_key < tree->min_key) {
		//插入到左树
		errno_t ret = InsertAvlEntry(tree->left, min_key, max_key, 
			data, data_len, &tree->left);
		if (ret != 0)
			//插入失败
			return ret;
		tree->left->parent = tree;
		if ((GetHeight(tree->left)- GetHeight(tree->right)) == 2) {
			//插入后出现不平衡
			if (min_key < tree->left->min_key) {
				//左左的情况, 向右单向旋转调整
				tree = LLRotate(tree);
			} else {
				//左右的情况, 先右旋, 再左旋
				tree = LRRotate(tree);
			}
		}		
	} else {
		//插入到右树
		errno_t ret = InsertAvlEntry(tree->right, min_key, max_key, 
			data, data_len, &tree->right);
		if (ret != 0)
			return ret;
		tree->right->parent = tree;
		if ((GetHeight(tree->right) - GetHeight(tree->left)) == 2) {
			//插入后出现不平衡
			if (min_key > tree->right->min_key) {
				//右右的情况, 向左单向旋转调整
				tree = RRRotate(tree);
			} else {
				//右左的情况, 先右旋, 再左旋
				tree = RLRotate(tree);
			}
		}
	}

	//调整子树自身的高度
	tree->height = MAX(GetHeight(tree->left), GetHeight(tree->right)) + 1;
	*new_root = tree;
	return 0;
}

avltree_t* GetMinAvlEntry(avltree_t* tree)
{
	if (tree == NULL) {
		return NULL;
	}

	while(tree->left)
		tree = tree->left;
	return tree;
}

avltree_t* GetMaxAvlEntry(avltree_t* tree)
{
	if (tree == NULL) {
		return NULL;
	}

	while(tree->right)
		tree = tree->right;
	return tree;
}

avltree_t* FindAvlEntry(avltree_t* tree, uint32_t key)
{
	if (tree == NULL) {
		return NULL;
	} else if (key >= tree->min_key && key <= tree->max_key) {
		//有重叠部分, 则不进行插入, 也不做任何调整
		return tree;
	} else if(key < tree->min_key) {
		//搜索左树
		avltree_t* found = FindAvlEntry(tree->left, key);
		return found;
	} else {
		//插入到右树
		avltree_t* found = FindAvlEntry(tree->right, key);
		return found;
	}
}

avltree_t* RebalanceAvlTree(avltree_t* tree)
{
	int left_height = GetHeight(tree->left);
	int right_height = GetHeight(tree->right);

	if ((left_height - right_height) == 2) {
		if (GetHeight(tree->left->left) > GetHeight(tree->left->right)) {
			return LLRotate(tree);
		} else {
			return LRRotate(tree);
		}
	} else if ((left_height - right_height) == -2 ) {
		if (GetHeight(tree->right->right) > GetHeight(tree->right->left)) {
			return RRRotate(tree);
		} else {
			return RLRotate(tree);
		}
	}
}

errno_t RemoveAvlEntry(avltree_t* tree, uint32_t min_key, avltree_t** new_tree)
{
	if ( new_tree == NULL) {
		return EINVAL;
	}

	if (tree == NULL) {
		//空树
		return ENOENT;
	} else if (min_key == tree->min_key) {
		//删除根节点
		//从右树中选出最小的一个节点作为新的根节点
		avltree_t* new_root = GetMinAvlEntry(tree->right);
		if (new_root == NULL) {
			//右树已经是空的了
			new_root = GetMaxAvlEntry(tree->left);
			if (new_root == NULL) {
				//左树也是空的了
				*new_tree = new_root;
				return 0;
			}
		}

		if (new_root->left != NULL) {
			new_root->parent->right = new_root->left;
		} else if (new_root->right != NULL) {
			new_root->parent->left = new_root->right;
		} else {
			if (new_root->parent->left == new_root)
				new_root->parent->left = NULL;
			else {
				new_root->parent->right == NULL;
			}
		}

		new_root->parent = NULL;
		new_root->left = tree->left;
		new_root->right = tree->right;
		free(tree);
		tree = new_root;
	} else if (min_key < tree->min_key) {
		errno_t ret = RemoveAvlEntry(tree->left, min_key, &tree->left);
		if (ret != 0) {
			//删除失败
			return ret;
		}
	} else {
		//从右树中删除
		errno_t ret = RemoveAvlEntry(tree->right, min_key, &tree->right);
		if (ret != 0) {
			return ret;
		}
	}
	*new_tree = RebalanceAvlTree(tree);
	return 0;
}

/* 清空AVL数 */
void ClearAvlTree(avltree_t* tree)
{
	if (tree == NULL ){
		return;
	}

	ClearAvlTree(tree->left);
	ClearAvlTree(tree->right);
	free(tree);
	tree = NULL;
}

/* 从文件中加载avltree */
errno_t LoadAvlTree(
	avltree_t* tree, 
	FILE* file, 
	int32_t size, 
	avltree_t** new_root)
{
	while(size > 0) {
		//遍历树
		uint32_t min_key = 0;
		uint32_t max_key = 0;
		int      data_len = 0;
		int read_len = 0;
		read_len = fread(&min_key, sizeof(min_key), 1, file);
		if (read_len != sizeof(min_key)) {
			return errno;
		}
		read_len = fread(&max_key, sizeof(max_key), 1, file);
		if (read_len != sizeof(max_key)) {
			return errno;
		}
		read_len = fread(&data_len, sizeof(data_len), 1, file);
		if (read_len != sizeof(data_len)) {
			return errno;
		}

		char* data = (char*)malloc(data_len);
		if (data == NULL) {
			return ENOMEM;
		}
		memset(data, 0, data_len);
		fread(data, data_len, 1, file);
		errno_t ret = InsertAvlEntry(tree, min_key, max_key, 
			data, data_len, new_root);
		free(data);
		data = NULL;
		if (ret != 0) {
			return ret;
		}
		tree = *new_root;
		size -= sizeof(min_key) + sizeof(max_key) + sizeof(data_len) + data_len;		
	}
	return 0;
}

/* 将AVL树保存到文件中*/
errno_t SaveAvlTree(avltree_t* tree, FILE* file, int32_t* size)
{
	if (tree== NULL) {
		return 0;
	}
	SaveAvlTree(tree->left, file, size);
	fwrite(&tree->min_key, sizeof(tree->min_key), 1, file);
	fwrite(&tree->max_key, sizeof(tree->max_key), 1, file);
	fwrite(&tree->data_len, sizeof(tree->data_len), 1, file);
	fwrite(tree->data, tree->data_len, 1, file);
	*size += sizeof(tree->min_key) + sizeof(tree->max_key) 
		+ sizeof(tree->data_len) + tree->data_len;
	SaveAvlTree(tree->right, file, size);
	return 0;
}