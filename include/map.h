#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "include/all.h"

#define M_OPQ  0
#define M_TSPT 1

#define ACS_WALL 255

enum DTile
{
	DGN_GROUND = 0,
	DGN_GRASS1,
	DGN_GRASS2,
	DGN_WALL,
	DGN_WALL1,
	DGN_WALL2,
	DGN_ROCK,
	DGN_AIR,
	DGN_DOWNSTAIR,
	DGN_UPSTAIR,
	DGN_TREE,
	DGN_FLOWER1,
	DGN_FLOWER2,
	DGN_CORRIDOR,
	DGN_DOOR,
	DGN_OPENDOOR,
	DGN_SLIME
};

struct map_item_struct
{
	char name[20];
	glyph gl;
	uint32_t attr;
};

struct DLevel;
extern struct map_item_struct map_items[];

int map_passable (struct DLevel *, int z, int y, int x);
int map_bpassable (struct DLevel *, int);

#endif /* MAP_H_INCLUDED */

