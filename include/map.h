#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "include/all.h"
#include "include/graphics.h"

#define M_OPQ  0
#define M_TSPT 1

#define ACS_WALL 255

#define PANE_H 10

typedef uint32_t SqAttr;

struct map_item_struct
{
	char name[20];
	char ch;
	uint32_t attr;
};

extern struct map_item_struct map_items[];
extern Graph map_graph;
int map_buffer (int, int);
int GETMAPITEMID(char);

#endif /* MAP_H_INCLUDED */

