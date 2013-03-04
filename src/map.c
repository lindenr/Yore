/* map.c */

#include "include/all.h"

#include "include/map.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/generate.h"
#include "include/graphics.h"

#define MAP_MOVEABLE 3

/* remember -- ONLY ONE MONSTER PER SQUARE */
void *get_sqmons (Vector *things, int yloc, int xloc)
{
	int n = gr_buffer(yloc, xloc);
	LOOP_THING(things, n, i)
	{
		struct Thing *th = THING(things, n, i);
		if (th->type == THING_MONS)
			return th;
	}
	/* no monster */
	return NULL;
}

uint32_t get_sqattr (Vector *things, int yloc, int xloc)
{
	uint32_t mvbl = 1;

	if (yloc >= MAP_HEIGHT || xloc >= MAP_WIDTH)
		return -1;

	int n = gr_buffer (yloc, xloc);
	LOOP_THING(things, n, i)
	{
		struct Thing *th = THING(things, n, i);
		if (th->type == THING_MONS)
		{
			mvbl = 2; /* attack */
			break;
		}
		if (th->type == THING_DGN)
		{
			if ((th->thing.mis.attr & 1) == 0)
			{
				mvbl = 0; /* unmoveable */
			}
		}
	}
	return mvbl;
}

int can_amove (int attr)
{
	if (attr == (uint32_t) - 1)
		return attr;
	return (attr & MAP_MOVEABLE);
}

#define MAPITEM(nm,ch,at,cl) {nm,ch,at}

struct map_item_struct map_items[] = {
	MAPITEM("lit space", ACS_BULLET, M_TSPT, 0),
	MAPITEM("wall", ACS_WALL, M_OPQ, 0),
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
