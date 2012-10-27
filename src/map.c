/* map.c */

#include "include/all.h"
#include <stdint.h>

#include "include/map.h"
#include "include/monst.h"
#include "include/thing.h"
#include "include/generate.h"
#include "include/graphics.h"

#define MAP_MOVEABLE 3

/* remember -- ONLY ONE MONSTER PER SQUARE */
struct Monster *get_square_monst (uint32_t yloc, uint32_t xloc, int level)
{
	ITER_THING(i, to_buffer(yloc, xloc))
	{
		struct Thing *th = i->data;
		if (th->type == THING_MONS)
			return (((struct Thing *)i->data)->thing);
	}
	/* no monster */
	return NULL;
}

uint32_t get_square_attr (uint32_t yloc, uint32_t xloc, int level)
{
	uint32_t mvbl = 1;

	if (yloc > 20 || xloc > 79)
		return -1;
	ITER_THINGS(i, n)
	{
		struct Thing *th = i->data;
		if (th->yloc == yloc && th->xloc == xloc)
		{
			if (th->type == THING_MONS)
			{
				mvbl = 2;		/* attack */
				break;
			}
			if (th->type == THING_DGN)
			{
				if ((((struct map_item_struct *)(th->thing))->attr & 1) == 0)
				{
					mvbl = 0;	/* unmoveable */
				}
			}
		}
	}
	return mvbl;
}

uint32_t can_move_to(uint32_t attr)
{
	if (attr == (uint32_t) - 1)
		return (uint32_t) - 1;
	return (attr & MAP_MOVEABLE);
}

#define MAPITEM(nm,ch,at,cl) {nm,ch,at}

struct map_item_struct map_items[] = {
	MAPITEM("lit space", ACS_BULLET, M_TSPT, 0),
	MAPITEM("wall", 'W', M_OPQ, 0),
	MAPITEM("downstair", '>', M_TSPT, 0),
	MAPITEM("upstair", '<', M_TSPT, 0),
	MAPITEM("tree", '+', M_OPQ, 0),
	MAPITEM("flower", '*', M_TSPT, 0),
	MAPITEM("corridor", '#', M_TSPT, 0),
	MAPITEM("_end_", '\0', M_OPQ, 0)
};

int GETMAPITEMID(char i)
{
	int n;

	for (n = 0; map_items[n].ch != '\0'; ++n)
	{
		if (map_items[n].ch == i)
			return n;
	}
	return n;
}
