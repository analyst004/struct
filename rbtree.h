/*
  Red Black Trees
  (C) 1999  Andrea Arcangeli <andrea@suse.de>
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  rbtree.h

  To use rbtrees you'll have to implement your own insert and search cores.
  This will avoid us to use callbacks and to drop drammatically performances.
  I know it's not the cleaner way,  but in C (not in C++) to get
  performances and genericity...

  See Documentation/rbtree.txt for documentation and samples.
*/

#ifndef	_RBTREE_H
#define	_RBTREE_H

#include "typedef.h"
#ifdef __cplusplus
extern "C" {
#endif

struct rb_node {
	unsigned long  __rb_parent_color;
	struct rb_node *rb_right;
	struct rb_node *rb_left;
} /*__attribute__((aligned(sizeof(long))))*/;
    /* The alignment might seem pointless, but allegedly CRIS needs it */

struct rb_root {
	struct rb_node *rb_node;
};


#define rb_parent(r)   ((struct rb_node *)(intptr_t)((r)->__rb_parent_color & ~3))

#define RB_ROOT	(struct rb_root) { NULL, }
#define	rb_entry(ptr, type, member) container_of(ptr, type, member)

#define RB_EMPTY_ROOT(root)  ((root)->rb_node == NULL)

/* 'empty' nodes are nodes that are known not to be inserted in an rbree */
#define RB_EMPTY_NODE(node)  \
	((node)->__rb_parent_color == (unsigned long)(intptr_t)(node))
#define RB_CLEAR_NODE(node)  \
	((node)->__rb_parent_color = (unsigned long)(intptr_t)(node))


void rb_insert_color(struct rb_node *, struct rb_root *);
void rb_erase(struct rb_node *, struct rb_root *);


/* Find logical next and previous nodes in a tree */
struct rb_node *rb_next(const struct rb_node *);
struct rb_node *rb_prev(const struct rb_node *);
struct rb_node *rb_first(const struct rb_root *);
struct rb_node *rb_last(const struct rb_root *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
void rb_replace_node(struct rb_node *victim, struct rb_node *new_node, 
			    struct rb_root *root);

static void rb_link_node(
    struct rb_node * node, 
    struct rb_node * parent,
		struct rb_node ** rb_link)
{
	node->__rb_parent_color = (unsigned long)(intptr_t)parent;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}

#define rb_find(root, compare, key, member_key, type, member) \
({ \
    struct rb_node *node = root->rb_node; \
    while (node) { \
        type* data = container_of(node, type, member); \
        int result = (compare)(key, data->member_key); \
        if (result < 0) node = node->rb_left;  \
        else if (result > 0) node = node->rb_right; \
        else return data; \
    } \
    return NULL;  \
})

#ifdef __cplusplus
}
#endif


#endif	/* _RBTREE_H */
