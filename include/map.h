#ifndef MAP_H_INCLUDED
#define MAP_H_INCLUDED

#include "include/all.h"
#include "include/graphics.h"

#define M_OPQ  0
#define M_TSPT 1

#define ACS_WALL 255

#define PANE_H 10

#define map_buffer(y,x) (gra_buffer (map_graph, (y), (x)))

typedef uint32_t SqAttr;

enum
{
	DGN_GROUND = 0,
	DGN_GRASS1,
	DGN_GRASS2,
	DGN_WALL,
	DGN_DOWNSTAIR,
	DGN_UPSTAIR,
	DGN_TREE,
	DGN_FLOWER1,
	DGN_FLOWER2,
	DGN_CORRIDOR,
	DGN_DOOR,
	DGN_OPENDOOR
};

struct map_item_struct
{
	char name[20];
	glyph gl;
	SqAttr attr;
};

struct DLevel;
extern struct map_item_struct map_items[];
void  *get_sqmons      (struct DLevel *, int, int);     /* returns the monster on a square     */
SqAttr get_sqattr      (struct DLevel *, int, int);     /* returns the SqAttr of a square      */
extern Graph map_graph;
int GETMAPITEMID(char);

#endif /* MAP_H_INCLUDED */

