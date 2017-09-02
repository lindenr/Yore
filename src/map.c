/* map.c */

#include "include/all.h"

#include "include/map.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/generate.h"
#include "include/graphics.h"

#define MAP_MOVEABLE 3

Graph map_graph = NULL;

int map_buffer (int yloc, int xloc)
{
	return map_graph->w*yloc + xloc;
}

/* remember -- ONLY ONE MONSTER PER SQUARE */
void *get_sqmons (Vector *things, int yloc, int xloc)
{
	int n = map_buffer (yloc, xloc);
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

	if (yloc >= map_graph->h || xloc >= map_graph->w)
		return -1;

	int n = map_buffer (yloc, xloc);
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

#define MAPITEM(nm,gl,at) {nm,gl,at}

struct map_item_struct map_items[] = {
	MAPITEM("lit space", ACS_BULLET, M_TSPT),
	MAPITEM("grass",     ACS_BULLET|COL_BG_RGB(0,5,0)|COL_TXT_DEF, M_TSPT),
	MAPITEM("grass",     ACS_BULLET|COL_BG_RGB(1,6,0)|COL_TXT_DEF, M_TSPT),
	MAPITEM("wall",      ACS_WALL,   M_OPQ),
	MAPITEM("downstair", '>',        M_TSPT),
	MAPITEM("upstair",   '<',        M_TSPT),
	MAPITEM("tree",      5|COL_TXT_RGB(2,1,0)|COL_BG_RGB(0,0,0),        M_TSPT),
	MAPITEM("flower",    '*'|COL_TXT_RGB(0,8,15),        M_TSPT),
	MAPITEM("flower",    '*'|COL_TXT_RGB(12,12,3),        M_TSPT),
	MAPITEM("corridor",  '#',        M_TSPT),
	MAPITEM("_end_",     '\0',       M_OPQ)
};

