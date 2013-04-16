#ifndef __PERF_INTLIST_H
#define __PERF_INTLIST_H

#include "rbtree.h"
#include "rblist.h"

typedef struct int_node {
	struct rb_node rb_node;
	int i;
}intnode_t;

typedef struct intlist {
	struct rblist rblist;
}intlist_t;

intlist_t *intlist__new(void);
void intlist__delete(intlist_t *ilist);

void intlist__remove(intlist_t *ilist, struct int_node *in);
int intlist__add(intlist_t *ilist, int i);

intnode_t *intlist__entry(const intlist_t *ilist, unsigned int idx);
intnode_t *intlist__find(intlist_t *ilist, int i);

static inline bool intlist__has_entry(intlist_t *ilist, int i)
{
	return intlist__find(ilist, i) != NULL;
}

static inline bool intlist__empty(const intlist_t *ilist)
{
	return rblist__empty(&ilist->rblist);
}

static inline unsigned int intlist__nr_entries(const intlist_t *ilist)
{
	return rblist__nr_entries(&ilist->rblist);
}

/* For intlist iteration */
static inline intnode_t *intlist__first(intlist_t *ilist)
{
	struct rb_node *rn = rb_first(&ilist->rblist.entries);
	return rn ? rb_entry(rn, intnode_t, rb_node) : NULL;
}
static inline intnode_t *intlist__next(intnode_t *in)
{
	struct rb_node *rn;
	if (!in)
		return NULL;
	rn = rb_next(&in->rb_node);
	return rn ? rb_entry(rn, struct int_node, rb_node) : NULL;
}

/**
 * intlist_for_each      - iterate over a intlist
 * @pos:	the &struct int_node to use as a loop cursor.
 * @ilist:	the &struct intlist for loop.
 */
#define intlist__for_each(pos, ilist)	\
	for (pos = intlist__first(ilist); pos; pos = intlist__next(pos))

/**
 * intlist_for_each_safe - iterate over a intlist safe against removal of
 *                         int_node
 * @pos:	the &struct int_node to use as a loop cursor.
 * @n:		another &struct int_node to use as temporary storage.
 * @ilist:	the &struct intlist for loop.
 */
#define intlist__for_each_safe(pos, n, ilist)	\
	for (pos = intlist__first(ilist), n = intlist__next(pos); pos;\
	     pos = n, n = intlist__next(n))
#endif /* __PERF_INTLIST_H */
