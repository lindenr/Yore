#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "include/all.h"
#include "include/graphics.h"

#define M_OPQ  0
#define M_TSPT 1

struct map_item_struct
{
	char name[20];
	char ch;
	uint32_t attr;
};

extern struct map_item_struct map_items[];
int GETMAPITEMID(char);

#endif /* MAP_H_INCLUDED */
