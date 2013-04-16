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
#include "slist.h"
#include "rbtree.h"
#include "strlist.h"

#ifdef __GNUC__
#ifdef __MINGW32__

#else
//linux or linux
#define _snprintf snprintf
#define _snwprintf swprintf
#endif
#endif

typedef struct _node_t
{
	int32_t		id;
	snode_t 	snode;
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

/*
node_t* my_remove(struct rb_root* root, char* name)
{
	node_t* node = (node_t*)rb_find(root, strcmp, name, name, node_t, rbnode);
	if (node) {
		rb_erase(&node->rbnode, root);

	} 
	return node;
}
*/

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

	//node = my_remove(&root, "N5823");
	//free(node);
	node = NULL;

	int count = 0;
	node_t *pos, *next = my_first(&root);
	while (next) {
		pos = next;
		next = my_next(pos);
		//my_remove(&root, pos->name);
		rb_erase(&pos->rbnode, &root);
		free(pos);
		count++;
	}
	printf("Count :%d\n", count);
}

void test_strlist()
{
	 strlist_t* list = strlist_new(true, NULL);
	 strlist_add(list, "AAAAAA");
	 strlist_add(list, "BBBBBB");
	 strlist_add(list, "CCCCCC");
	 strlist_add(list, "DDDDDD");
	 strlist_add(list, "EEEEEE");
	 strlist_add(list, "FFFFFF");

	 unsigned int count = strlist_nr_entries(list);
	 printf("strlist count: %d\n", count);
	 for( unsigned int i=0; i<count; i++) {
	 	strnode_t* node = strlist_entry(list, i);
	 	printf("strlist node: %s\n", node->s);
	 }


	 for(strnode_t* it = strlist_first(list);
	 	 it != NULL; 
	 	 it = strlist_next(it)) {
	 	printf("strlist node: %s\n", it->s);
	 }

	 strnode_t* node = strlist_find(list, "DDDDDD");
	 printf("find string node:%s", node->s);

	 strlist_delete(list);
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

void test_slist()
{
	slist_t list = {0};
	slist_init(&list);

	for (int i =0; i < 100; i++) {
		node_t* node = (node_t*)malloc(sizeof(node_t));
		memset(node, 0, sizeof(node_t));
		node->id = i;
		slist_add(&list, &node->snode);
	}
	
	node_t* pos = NULL;
	slist_for_each_safe(pos, &list, node_t, snode) {
		free(pos);
		pos = NULL;
	}
}

int main(int argc, char* argv[])
{
	while(true) {
	test_slist();
	test_dlist();
	test_hashtable();
	test_rbtree();
	test_strlist();
	}
}