#ifndef _HASH_MAP_H_KIMZHANG
#define _HASH_MAP_H_KIMZHANG

#include "hlist.h"

#define HASH_BURKET_SIZE	1024

typedef struct _hash_map
{
	hlist_t burkets[HASH_BURKET_SIZE];
}hash_map_t;

typedef struct _hash_map_node
{
	uint32_t	key;
	hnode_t     node;
}hash_map_node_t;

static inline
void hash_map_init(hash_map_t* hash_map)
{
	htable_init(hash_map->burkets, HASH_BURKET_SIZE);
}

static inline
bool hash_map_insert(hash_map_t* hash_map, unsigned int key, hash_map_node_t* entry)
{
	hlist_t* list = htable_burket(hash_map->burkets, HASH_BURKET_SIZE, key);
	entry->key = key;
	hlist_add_head(&entry->node, list);
	return true;
}

static inline
hash_map_node_t* hash_map_find(hash_map_t* hash_map, unsigned int key)
{
	hlist_t* list = htable_burket(hash_map->burkets, HASH_BURKET_SIZE, key);
	hnode_t* it = NULL;
	hlist_for_each(it, list) {
		hash_map_node_t* entry = hlist_entry(it, hash_map_node_t, node);
		if (entry->key == key)
			return entry;
	}
	return NULL;
}

static inline
hash_map_node_t*  hash_map_earse(hash_map_t* hash_map, unsigned int key)
{
	hash_map_node_t* entry = hash_map_find(hash_map, key);
	if (entry == NULL)
		return NULL;

	hlist_del(&entry->node);
	return entry;
}

#define hash_map_for_each(pos, hash_map)	\
	for (int i=0; i<HASH_BURKET_SIZE; i++) {	\
		hnode_t* it = NULL;		\
		hlist_for_each(it, &hash_map[i]) {	\
			pos = hlist_entry(it, hash_map_node_t, node);

#define hash_map_end_each \
		}	\
	}

#endif //_HASH_MAP_H_KIMZHANG