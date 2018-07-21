/* map.c */

#include "include/all.h"

#include "include/map.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/generate.h"
#include "include/graphics.h"
#include "include/dlevel.h"

Graph map_graph = NULL;

int map_bpassable (struct DLevel *lvl, int n)
{
	int i;
	for (i = 0; i < lvl->things[n]->len; ++ i)
	{
		struct Thing *th = THING(lvl->things, n, i);
		if (th->type == THING_DGN && (th->thing.mis.attr & 1) == 0)
			return 0;
	}
	return 1;
}

int map_passable (struct DLevel *lvl, int y, int x)
{
	return map_bpassable (lvl, map_buffer (y, x));
}

#define MAPITEM(nm,gl,at) {(nm), (gl), (at)}

struct map_item_struct map_items[] = {
	MAPITEM("lit space", ACS_BIGDOT,                        M_TSPT),
	MAPITEM("grass",     ACS_DOT|COL_BG(0,5,0)|COL_TXT_DEF, M_TSPT),
	MAPITEM("grass",     ACS_DOT|COL_BG(1,6,0)|COL_TXT_DEF, M_TSPT),
	MAPITEM("wall",      ACS_WALL,                          M_OPQ ),
	MAPITEM("rock",      ' ',                               M_OPQ ),
	MAPITEM("downstair", '>',                               M_TSPT),
	MAPITEM("upstair",   '<',                               M_TSPT),
	MAPITEM("tree",      5  |COL_TXT(2,1,0)|COL_BG(0,0,0),  M_TSPT),
	MAPITEM("flower",    '*'|COL_TXT(0,8,15),               M_TSPT),
	MAPITEM("flower",    '*'|COL_TXT(12,12,3),              M_TSPT),
	MAPITEM("corridor",  ACS_CORRIDOR,                      M_TSPT),
	MAPITEM("door",      '+'|COL_TXT(9,9,0),                M_OPQ ),
	MAPITEM("open door", 254|COL_TXT(9,9,0),                M_TSPT),
	MAPITEM("slime",     0xB2|COL_TXT(3,9,0),               M_TSPT),
	MAPITEM("_end_",     '\0',                              M_OPQ )
};

