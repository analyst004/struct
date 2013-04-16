/*
 * (c) 2009 Arnaldo Carvalho de Melo <acme@redhat.com>
 *
 * Licensed under the GPLv2.
 */
#pragma warning(disable:4996)
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "typedef.h"
#include "contain.h"
#include "strlist.h"



static
struct rb_node *strlist_node_new(struct rblist *rblist, const void *entry)
{
	const char *s = (const char*)entry;
	struct rb_node *rc = NULL;
	strlist_t *strlist = container_of(rblist, strlist_t, rblist);
	strnode_t *snode = (strnode_t*)malloc(sizeof(*snode));

	if (snode != NULL) {
		if (strlist->dupstr) {
			s = strdup(s);
			if (s == NULL)
				goto out_delete;
		}
		snode->s = s;
		rc = &snode->rb_node;
	}

	return rc;

out_delete:
	free(snode);
	return NULL;
}

static void str_node__delete(strnode_t *self, bool dupstr)
{
	if (dupstr)
		free((void *)self->s);
	free(self);
}

static
void strlist_node_delete(struct rblist *rblist, struct rb_node *rb_node)
{
	strlist_t *slist = container_of(rblist, strlist_t, rblist);
	strnode_t *snode = container_of(rb_node, strnode_t, rb_node);

	str_node__delete(snode, slist->dupstr);
}

static int strlist_node_cmp(struct rb_node *rb_node, const void *entry)
{
	const char *str = (const char*)entry;
	strnode_t *snode = container_of(rb_node, strnode_t, rb_node);

	return strcmp(snode->s, str);
}

int strlist_add(strlist_t *self, const char *new_entry)
{
	return rblist__add_node(&self->rblist, new_entry);
}

int strlist_load(strlist_t *self, const char *filename)
{
	char entry[1024];
	int err;
	FILE *fp = fopen(filename, "r");

	if (fp == NULL)
		return errno;

	while (fgets(entry, sizeof(entry), fp) != NULL) {
		const size_t len = strlen(entry);

		if (len == 0)
			continue;
		entry[len - 1] = '\0';

		err = strlist_add(self, entry);
		if (err != 0)
			goto out;
	}

	err = 0;
out:
	fclose(fp);
	return err;
}

void strlist_remove(strlist_t *slist, strnode_t *snode)
{
	rblist__remove_node(&slist->rblist, &snode->rb_node);
}

strnode_t *strlist_find(strlist_t *slist, const char *entry)
{
	strnode_t *snode = NULL;
	struct rb_node *rb_node = rblist__find(&slist->rblist, entry);

	if (rb_node)
		snode = container_of(rb_node, strnode_t, rb_node);

	return snode;
}

static int strlist_parse_list_entry(strlist_t *self, const char *s)
{
	if (strncmp(s, "file://", 7) == 0)
		return strlist_load(self, s + 7);

	return strlist_add(self, s);
}

int strlist_parse_list(strlist_t *self, const char *s)
{
	char *sep;
	int err;

	while ((sep = (char*)strchr(s, ',')) != NULL) {
		*sep = '\0';
		err = strlist_parse_list_entry(self, s);
		*sep = ',';
		if (err != 0)
			return err;
		s = sep + 1;
	}

	return *s ? strlist_parse_list_entry(self, s) : 0;
}

strlist_t *strlist_new(bool dupstr, const char *slist)
{
	strlist_t *self = (strlist_t*)malloc(sizeof(*self));

	if (self != NULL) {
		rblist__init(&self->rblist);
		self->rblist.node_cmp    = strlist_node_cmp;
		self->rblist.node_new    = strlist_node_new;
		self->rblist.node_delete = strlist_node_delete;

		self->dupstr	 = dupstr;
		if (slist && strlist_parse_list(self, slist) != 0)
			goto out_error;
	}

	return self;
out_error:
	free(self);
	return NULL;
}

void strlist_delete(strlist_t *self)
{
	if (self != NULL)
		rblist__delete(&self->rblist);
}

strnode_t *strlist_entry(const strlist_t *slist, unsigned int idx)
{
	strnode_t *snode = NULL;
	struct rb_node *rb_node;

	rb_node = rblist__entry(&slist->rblist, idx);
	if (rb_node)
		snode = container_of(rb_node, strnode_t, rb_node);

	return snode;
}
