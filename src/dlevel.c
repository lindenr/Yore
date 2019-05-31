/* dlevel.c */

#include "include/dlevel.h"
#include "include/thing.h"
#include "include/graphics.h"
#include "include/heap.h"
#include "include/rand.h"
#include "include/panel.h"
#include "include/vector.h"
#include "include/monst.h"
#include "include/world.h"
#include "include/generate.h"
#include "include/event.h"

#include <string.h>

int dlv_init (int uplevel, int dnlevel, int z, int y, int x)
{
	int i;
	int v = z*y*x;
	int dlevel = world.dlevels->len;
	struct DLevel new_level = {
		dlevel,
		z, y, x, y*x, v,
		malloc (sizeof(DTile) * v),
		malloc (sizeof(V_ItemID) * v),
		malloc (sizeof(MonsID) * v),
		malloc (sizeof(int) * v),
		malloc (sizeof(int) * v),
		malloc (sizeof(int) * v),
		malloc (sizeof(uint8_t) * v),
		malloc (sizeof(uint8_t) * v),
		malloc (sizeof(glyph) * v),
		uplevel,
		dnlevel
	};
	/*struct DLevel *lvl = dlv_internal (uplevel);
	if (lvl)
		lvl->dnlevel = dlevel;
	lvl = dlv_internal (dnlevel);
	if (lvl)
		lvl->uplevel = dlevel;*/
	for (i = 0; i < v; ++ i)
	{
		new_level.itemIDs[i] = v_dinit (sizeof(ItemID));
		new_level.player_dist[i] = -1;
		new_level.escape_dist[i] = -1;
	}
	memset (new_level.tiles, 0, sizeof(DTile)*v);
	memset (new_level.monsIDs, 0, sizeof(MonsID)*v);
	memset (new_level.num_fires, 0, sizeof(int)*v);
	memset (new_level.seen, 0, sizeof(uint8_t)*v);
	memset (new_level.attr, 0, sizeof(uint8_t)*v);
	memset (new_level.remembered, 0, sizeof(glyph)*v);
	v_push (world.dlevels, &new_level);
	generate_map (dlevel, LEVEL_3D);
	ev_queue (0, dlevel_heartbeat, dlevel);
	return dlevel;
}

int dlv_index (int dlevel, int z, int y, int x)
{
	struct DLevel *lvl = dlv_internal (dlevel);
	if (z < 0 || z >= lvl->t ||
		y < 0 || y >= lvl->h ||
		x < 0 || x >= lvl->w)
		return -1;
	return lvl->a * z + lvl->w * y + x;
}

struct DLevel *dlv_internal (int dlevel)
{
	struct DLevel *lvl;
	//if (dlevel == -1)
	//	return NULL;
	lvl = &world.dlevels->data[dlevel];
	if (lvl->dlevel == dlevel)
		return lvl;
	return NULL;
}

V_ItemID dlv_items (int dlevel, int z, int y, int x)
{
	return dlv_internal (dlevel)->itemIDs[dlv_index (dlevel, z, y, x)];
}

/*uint8_t *dlv_attr (int dlevel)
{
	return dlv_internal (dlevel)->attr;
}

int dlv_up (int dlevel)
{
	return dlv_internal (dlevel)->uplevel;
}

int dlv_dn (int dlevel)
{
	return dlv_internal (dlevel)->dnlevel;
}*/

void dlv_settile (int dlevel, int z, int y, int x, DTile tile)
{
	dlv_internal (dlevel)->tiles[dlv_index (dlevel, z, y, x)] = tile;
}

DTile dlv_tile (int dlevel, int z, int y, int x)
{
	int i = dlv_index (dlevel, z, y, x);
	if (i == -1)
		return DGN_ROCK;
	return dlv_internal (dlevel)->tiles[i];
}

MonsID dlv_mons (int dlevel, int z, int y, int x)
{
	int i = dlv_index (dlevel, z, y, x);
	if (i == -1)
		return 0;
	return dlv_internal (dlevel)->monsIDs [i];
}

void dlv_setmons (int dlevel, int z, int y, int x, MonsID mons)
{
	dlv_internal (dlevel)->monsIDs [dlv_index (dlevel, z, y, x)] = mons;
}

int dlv_num_fires (int dlevel, int z, int y, int x)
{
	int i = dlv_index (dlevel, z, y, x);
	if (i == -1)
		return 0;
	return dlv_internal (dlevel)->num_fires [i];
}

void dlv_set_fires (int dlevel, int z, int y, int x, int nf)
{
	dlv_internal (dlevel)->num_fires [dlv_index (dlevel, z, y, x)] = nf;
}

#define M_OPQ  0
#define M_TSPT 1

int dlv_passable (int dlevel, int z, int y, int x)
{
	DTile t = dlv_tile (dlevel, z, y, x);
	return tile_types[t].attr&M_TSPT;
}

#define MAPITEM(nm,gl,at) {(nm), (gl), (at)}

struct DTile_type tile_types[] = {
	MAPITEM("lit space", ACS_BIGDOT|COL_BG(0,0,0)|COL_TXT_DEF,          M_TSPT),
	MAPITEM("grass",     ACS_BIGDOT|COL_BG(0,5,0)|COL_TXT_DEF, M_TSPT),
	MAPITEM("grass",     ACS_BIGDOT|COL_BG(1,6,0)|COL_TXT_DEF, M_TSPT),
	MAPITEM("wall",      0x999332FE,                          M_OPQ ),
	MAPITEM("wall",      0x999432FE,                          M_OPQ ),
	MAPITEM("wall",      0x999342FE,                          M_OPQ ),
	MAPITEM("rock",      ' ',                               M_OPQ ),
	MAPITEM("air",       0,                          M_TSPT ),
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

struct TileDist
{
	int y, x;
	int dist;
};

int cmp_tile (const struct TileDist *t1, const struct TileDist *t2)
{
	return t1->dist > t2->dist;
}

void dlv_tile_burn (int dlevel, int zloc, int yloc, int xloc)
{
	// mons_burn (mons); TODO
	MonsID mons = dlv_mons (dlevel, zloc, yloc, xloc);
	if (mons)
	{
		int damage = rn(3);
		eff_mons_burns (mons, damage);
		mons_take_damage (mons, 0, damage, DTYP_FIRE);
	}
	V_ItemID items = dlv_items (dlevel, zloc, yloc, xloc);
	int n;
	for (n = 0; n < items->len; ++ n)
		it_burn (items->data[n]);
}

#define MAX_DIST 10
#define ESC_FUDGE 3
#define ESC_REM   1
//static int *ylocs = NULL, *xlocs = NULL;
//static struct Heap *tiles = NULL;
void dlv_fill_player_dist (int dlevel)
{
	/*if (!ylocs) TODO better
	{
		int d = 2*MAX_DIST + 1;
		ylocs = malloc (sizeof(int)*d*d);
		xlocs = malloc (sizeof(int)*d*d);
		tiles = h_dinit (sizeof (struct TileDist), cmp_tile);
	}
	h_empty (tiles);
	int i, cur_loc = 0, cur_back;
	for (i = 0; i < map_graph->a; ++ i)
	{
		lvl->player_dist[i] = -1;
		lvl->escape_dist[i] = -1;
	}
	for (i = 0; i < lvl->playerIDs->len; ++ i)
	{
		MonsID player = lvl->playerIDs->data[i];
		ylocs[cur_loc] = player->yloc;
		xlocs[cur_loc] = player->xloc;
		++ cur_loc;
	}
	for (cur_back = 0; cur_back < cur_loc; ++ cur_back)
	{
		int y, x, min = MAX_DIST + 5;
		if (lvl->player_dist[map_buffer(ylocs[cur_back], xlocs[cur_back])] >= 0)
			continue;
		for (y = -1; y <= 1; ++ y) for (x = -1; x <= 1; ++ x)
		{
			if (y == 0 && x == 0)
				continue;
			int current = lvl->player_dist[map_buffer(ylocs[cur_back]+y, xlocs[cur_back]+x)];
			if (current < min && current >= 0)
				min = current;
		}
		if (min < MAX_DIST + 5)
		{
			lvl->player_dist[map_buffer(ylocs[cur_back], xlocs[cur_back])] = min + 1;
			lvl->escape_dist[map_buffer(ylocs[cur_back], xlocs[cur_back])] = ESC_FUDGE * (min + 1);
			struct TileDist td = {ylocs[cur_back], xlocs[cur_back], ESC_FUDGE * (min + 1)};
			h_push (tiles, &td);
		}
		else
			lvl->player_dist[map_buffer(ylocs[cur_back], xlocs[cur_back])] = 0;
		if (lvl->player_dist[map_buffer(ylocs[cur_back], xlocs[cur_back])] >= MAX_DIST)
			continue;
		for (y = -1; y <= 1; ++ y) for (x = -1; x <= 1; ++ x)
		{
			if (y == 0 && x == 0)
				continue;
			int current = lvl->player_dist[map_buffer(ylocs[cur_back]+y, xlocs[cur_back]+x)];
			if (current == -1 &&
				ylocs[cur_back]+y+1 < map_graph->h && ylocs[cur_back]+y-1 >= 0 &&
				xlocs[cur_back]+x+1 < map_graph->w && xlocs[cur_back]+x-1 >= 0 &&
				dlv_passable (cur_dlevel, ylocs[cur_back]+y, xlocs[cur_back]+x))
			{
				ylocs[cur_loc] = ylocs[cur_back]+y;
				xlocs[cur_loc] = xlocs[cur_back]+x;
				++ cur_loc;
			}
		}
	}
	while (tiles->len)
	{
		const struct TileDist *t = h_least (tiles);
		int y, x;
		for (y = -1; y <= 1; ++ y) for (x = -1; x <= 1; ++ x)
		{
			if (y == 0 && x == 0)
				continue;
			int yloc = t->y + y, xloc = t->x + x;
			if (yloc < 0 || yloc >= map_graph->h || xloc < 0 || xloc >= map_graph->w)
				continue;
			if (lvl->escape_dist[map_buffer(yloc, xloc)] == -1)
				continue;
			if (lvl->escape_dist[map_buffer(yloc, xloc)] >=
				lvl->escape_dist[map_buffer(t->y, t->x)] - ESC_REM)
				continue;
			lvl->escape_dist[map_buffer(yloc, xloc)] =
				lvl->escape_dist[map_buffer(t->y, t->x)] - ESC_REM;
			struct TileDist t1 = {yloc, xloc, lvl->escape_dist[map_buffer(yloc, xloc)]};
			h_push (tiles, &t1);
		}
		h_pop (tiles, NULL);
	}*/
}

