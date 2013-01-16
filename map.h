#ifndef MAP_H_KIMZHANG_
#define MAP_H_KIMZHANG_

#include "rbtree.h"

typedef struct _map_node
{
	uint32_t key;
	struct rb_node node;
}map_node_t;

typedef struct rb_root 	map_t;

static inline
map_node_t* map_find(map_t* map, uint32_t key)
{
    struct rb_node *it = map->rb_node;
    while (it) {
        map_node_t* data = container_of(it, map_node_t, node);

        int result = key - data->key;
        if (result < 0) it = it->rb_left;
        else if (result > 0) it = it->rb_right;
        else return data;
    }
    return NULL;
}

static inline
bool map_insert(map_t *map, uint32_t key, map_node_t *entry)
{
    struct rb_node **new_root = &(map->rb_node), *parent = NULL;

    while (*new_root)
    {
        map_node_t *curr = container_of(*new_root, map_node_t, node);

        int result = key - curr->key;

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

static inline
map_node_t* map_remove(map_t* map, uint32_t key)
{
	map_node_t* target = map_find(map, key);
	if (target == NULL) {
		return NULL;
	}

	rb_erase(&target->rbnode, map); 
	return target;
}

static inline
map_node_t* map_first(map_t* map)
{
	struct rb_node* entry = rb_first(map);
	if (entry == NULL)
		return NULL;
	return container_of(entry, map_node_t, node);
}

static inline
map_node_t* map_next(map_node_t* iter)
{
	struct rb_node *entry = rb_next(&iter->rbnode);
	if (entry == NULL)
		return NULL;
	return container_of(entry, map_node_t, node);
}

// #define map_for_each(it, map)	\
// 	for (it = map_first(map); it != NULL; it = map_next(it))

#define map_for_each_safe(it, map)	\
	for (map_node_t* tmp = NULL, *it = map_first(map); \
		(it != NULL) && (tmp = it->next); 	\
		it = tmp)

#endif // MAP_H_KIMZHANG_