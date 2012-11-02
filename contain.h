#ifndef _CONTAIN_H_KIMZHANG
#define _CONTAIN_H_KIMZHANG

#define offset_of(type, member) ((size_t) &((type *)0)->member)

#define container_of(ptr, type, member)     \
	(type *)( (char *)(ptr) - offset_of(type,member) )

#endif
