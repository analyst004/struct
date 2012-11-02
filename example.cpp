#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "typedef.h"
#include "jhash.h"
#include "dlist.h"
#include "hlist.h"
#include "rbtree.h"

typedef struct _node_t
{
	int32_t		id;
	dnode_t 	node;
	hnode_t 	hnode;
	struct rb_node 	rbnode;
	int 		value;
	char		name[32];
}node_t;

/*
node_t *my_search(struct rb_root *root, char* name)
{
    struct rb_node *node = root->rb_node;
    while (node) {
        node_t* data = container_of(node, node_t, rbnode);

        int result = strcmp(name, data->name);

        if (result < 0) node = node->rb_left;
        else if (result > 0) node = node->rb_right;
        else return data;
    }
    return NULL;
}
*/

bool my_insert(struct rb_root *root, node_t *data)
{
    struct rb_node **new_root = &(root->rb_node), *parent = NULL;

    while (*new_root)
    {
        node_t *curr = container_of(*new_root, node_t, rbnode);

        int result = strcmp(data->name, curr->name);

        parent = *new_root;

        if (result < 0)

            new_root = &((*new_root)->rb_left);

        else if (result > 0)

            new_root = &((*new_root)->rb_right);

        else

            return false;

    }


    rb_link_node(&data->rbnode, parent, new_root);
    rb_insert_color(&data->rbnode, root);

    return true;
}

node_t* my_remove(struct rb_root* root, char* name)
{
	node_t* node = rb_find(root, strcmp, name, name, node_t, rbnode);
	if (node) {
		rb_erase(&node->rbnode, root);

	} 
	return node;
}

node_t* my_first(struct rb_root* root)
{
	struct rb_node* node = rb_first(root);
	if (node == NULL)
		return NULL;
	return container_of(node, node_t, rbnode);
}

node_t* my_next(node_t* iter)
{
	struct rb_node *node = rb_next(&iter->rbnode);
	if (node == NULL)
		return NULL;
	return container_of(node, node_t, rbnode);
}


void test_rbtree()
{
	struct rb_root root = {NULL};
	for (int i=0; i < 10240; i++) {
		node_t* node = (node_t*)malloc(sizeof(node_t));
		memset(node, 0, sizeof(node_t));
		_snprintf(node->name, sizeof(node->name) - 1, "N%d", i);
		node->id = i;
		my_insert(&root, node);
	}

	node_t* node = NULL;
	for ( node = my_first(&root); node != NULL; node = my_next(node)) {
		printf("%s", node->name);
	}

	node = my_remove(&root, "N5823");
	free(node);
	node = NULL;

	int count = 0;
	node_t *pos, *next = my_first(&root);
	while (next) {
		pos = next;
		next = my_next(pos);
		my_remove(&root, pos->name);
		free(pos);
		count++;
	}
	printf("Count :%d\n", count);
}

void test_dlist()
{
	dlist_t 	list;
	dlist_init(&list);

	//insert elements
	for (int i = 0; i < 5; i++) {
		node_t* node = (node_t*)malloc(sizeof(node_t));
		if (node == NULL) {

		}
		memset(node, 0, sizeof(node_t));
		node->id = i;
		dlist_add_tail(&list, &node->node);
	}

	node_t* node = NULL;
	//printf("%s", typeid(*pos).name());

	dlist_for_each_entry(node, &list, node_t, node) {
		printf("%d", node->id);
	}
	printf("\n");
	
	node = NULL;
	for (node = dlist_first_entry(&list, node_t, node); 
		node != NULL;
		node = dlist_next_entry(node, &list, node_t, node)) {
		printf("%d", node->id);
	}
	printf("\n");

	node = NULL;
	dlist_for_each_entry_reverse(node, &list, node_t, node) {
		printf("%d", node->id);
	}
	printf("\n");

	node = NULL;
	node_t* tmp = NULL;
	dlist_for_each_entry_safe(node, tmp, &list, node_t, node) {
		free(node);
		node = NULL;
	}
}

void test_hashtable()
{
	INIT_HASH_TABLE(ht, 1024);

	for (int i=0; i < 10000; i++ ) {
		uint32_t hash = jhash(&i, 4, JHASH_INITVAL);
		node_t* node = (node_t*)malloc(sizeof(node_t));
		memset(node, 0, sizeof(node_t));
		node->id = i;
		hlist_t* burket = htable_burket(ht, 1024, hash);
		hlist_add_head(&node->hnode, burket);
	}

	for (int i=0; i < 1024; i++) {
		node_t* node = NULL;
		hlist_for_each_entry_safe(node, &ht[i], node_t, hnode) {
			free(node);
			node = NULL;
		}
	}	
}

int main(int argc, char* argv[])
{
	while(true) {
	test_dlist();
	test_hashtable();
	test_rbtree();
	}
}