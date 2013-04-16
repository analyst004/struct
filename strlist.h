#ifndef __PERF_STRLIST_H
#define __PERF_STRLIST_H

#include "rbtree.h"
#include "rblist.h"

typedef struct str_node {
	struct rb_node rb_node;
	const char     *s;
}strnode_t;

typedef struct strlist {
	struct rblist rblist;
	bool	       dupstr;
}strlist_t;

strlist_t *strlist_new(bool dupstr, const char *slist);
void strlist_delete(strlist_t *self);

void strlist_remove(strlist_t *self, strnode_t *sn);
int strlist_load(strlist_t *self, const char *filename);
int strlist_add(strlist_t *self, const char *str);

strnode_t *strlist_entry(const strlist_t *self, unsigned int idx);
strnode_t *strlist_find(strlist_t *self, const char *entry);

static inline bool strlist_has_entry(strlist_t *self, const char *entry)
{
	return strlist_find(self, entry) != NULL;
}

static inline bool strlist_empty(const strlist_t *self)
{
	return rblist__empty(&self->rblist);
}

static inline unsigned int strlist_nr_entries(const strlist_t *self)
{
	return rblist__nr_entries(&self->rblist);
}

/* For strlist iteration */
static inline strnode_t *strlist_first(strlist_t *self)
{
	struct rb_node *rn = rb_first(&self->rblist.entries);
	return rn ? (strnode_t*)rb_entry(rn, strnode_t, rb_node) : NULL;
}

static inline strnode_t *strlist_next(strnode_t *sn)
{
	struct rb_node *rn;
	if (!sn)
		return NULL;
	rn = rb_next(&sn->rb_node);
	return rn ? (strnode_t*)rb_entry(rn, strnode_t, rb_node) : NULL;
}

/**
 * strlist_for_each      - iterate over a strlist
 * @pos:	the &struct str_node to use as a loop cursor.
 * @self:	the &struct strlist for loop.
 */
#define strlist_for_each(pos, self)	\
	for (pos = strlist__first(self); pos; pos = strlist__next(pos))

/**
 * strlist_for_each_safe - iterate over a strlist safe against removal of
 *                         str_node
 * @pos:	the &struct str_node to use as a loop cursor.
 * @n:		another &struct str_node to use as temporary storage.
 * @self:	the &struct strlist for loop.
 */
#define strlist_for_each_safe(pos, n, self)	\
	for (pos = strlist__first(self), n = strlist__next(pos); pos;\
	     pos = n, n = strlist__next(n))

int strlist_parse_list(strlist_t *self, const char *s);
#endif /* __PERF_STRLIST_H */
