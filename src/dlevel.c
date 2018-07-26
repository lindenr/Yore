/* dlevel.c */

#include "include/dlevel.h"
#include "include/thing.h"
#include "include/graphics.h"
#include "include/heap.h"
#include "include/rand.h"
#include "include/panel.h"

// TODO: make this all non-global in a nice struct somewhere
Vector all_ids;
Vector all_dlevels;
int    cur_level;
struct DLevel *cur_dlevel;
char *player_name;

void dlv_init ()
{
	all_dlevels = v_dinit (sizeof(struct DLevel));

	void *t = NULL;
	all_ids = v_dinit (sizeof(void *));
	v_push (all_ids, &t);

	dlv_make (1, 0, 0);
	dlv_make (2, 1, 0);
	dlv_set (1);
}

void dlv_make (int level, int uplevel, int dnlevel)
{
	int i;
	struct DLevel new_level = {
		level,
		malloc (sizeof(Vector) * map_graph->a),
		malloc (sizeof(Vector) * map_graph->a),
		malloc (sizeof(TID) * map_graph->a),
		v_dinit (sizeof(struct Monster)),
		v_dinit (sizeof(TID)),
		malloc (sizeof(int) * map_graph->a),
		malloc (sizeof(int) * map_graph->a),
		malloc (sizeof(int) * map_graph->a),
		malloc (sizeof(uint8_t)*map_graph->a),
		malloc (sizeof(uint8_t)*map_graph->a),
		malloc (sizeof(glyph)*map_graph->a),
//		malloc (sizeof(int) * map_graph->a),
		uplevel,
		dnlevel
	};
	struct DLevel *lvl = dlv_lvl (uplevel);
	if (lvl)
		lvl->dnlevel = level;
	lvl = dlv_lvl (dnlevel);
	if (lvl)
		lvl->uplevel = level;
	for (i = 0; i < map_graph->a; ++ i)
	{
		new_level.things[i] = v_dinit (sizeof(struct Thing));
		new_level.items[i] = v_dinit (sizeof(struct Item));
		new_level.player_dist[i] = -1;
		new_level.escape_dist[i] = -1;
	}
	struct Monster dummy_mons = {0};
	v_push (new_level.mons, &dummy_mons);
	memset (new_level.monsIDs, 0, sizeof(TID)*map_graph->a);
	memset (new_level.num_fires, 0, sizeof(int)*map_graph->a);
	memset (new_level.seen, 0, sizeof(uint8_t)*map_graph->a);
	memset (new_level.attr, 0, sizeof(uint8_t)*map_graph->a);
	memset (new_level.remembered, 0, sizeof(glyph)*map_graph->a);
//	memset (new_level.descs, 0, sizeof(int)*map_graph->a);
	v_push (all_dlevels, &new_level);
}

void dlv_set (int level)
{
	cur_level = level;
	cur_dlevel = dlv_lvl (level);
}

struct DLevel *dlv_lvl (int level)
{
	int i;
	struct DLevel *lvl;
	if (level == 0)
		return NULL;
	for (i = 0; i < all_dlevels->len; ++ i)
	{
		lvl = v_at (all_dlevels, i);
		if (lvl->level == level)
			break;
	}
	if (i >= all_dlevels->len)
		return NULL;
	return lvl;
}

Vector *dlv_things (int level)
{
	return dlv_lvl (level)->things;
}

Vector *dlv_items (int level)
{
	return dlv_lvl (level)->items;
}

uint8_t *dlv_attr (int level)
{
	return dlv_lvl (level)->attr;
}

int dlv_up (int level)
{
	return dlv_lvl (level)->uplevel;
}

int dlv_dn (int level)
{
	return dlv_lvl (level)->dnlevel;
}

struct TileDist
{
	int y, x;
	int dist;
};

int cmp_tile (const struct TileDist *t1, const struct TileDist *t2)
{
	return t1->dist > t2->dist;
}

void dlv_tile_burn (struct DLevel *dlvl, int yloc, int xloc)
{
	// mons_burn (mons); TODO
	int i = map_buffer (yloc, xloc);
	struct Monster *mons = v_at (dlvl->mons, dlvl->monsIDs[i]);
	if (mons->ID)
	{
		int damage = rn(5);
		eff_mons_burns (mons, damage);
		mons_take_damage (mons, NULL, damage, DTYP_FIRE);
	}
	Vector items = dlvl->items[i];
	int n;
	for (n = 0; n < items->len; ++ n)
		it_burn (v_at (items, n));
}

#define MAX_DIST 10
#define ESC_FUDGE 3
#define ESC_REM   1
static int *ylocs = NULL, *xlocs = NULL;
static struct Heap *tiles = NULL;
void dlv_fill_player_dist (struct DLevel *dlv)
{
	if (!ylocs)
	{
		ylocs = malloc (sizeof(int)*map_graph->a);
		xlocs = malloc (sizeof(int)*map_graph->a);
		tiles = h_dinit (sizeof (struct TileDist), cmp_tile);
	}
	h_empty (tiles);
	int i, cur_loc = 0, cur_back;
	for (i = 0; i < map_graph->a; ++ i)
	{
		dlv->player_dist[i] = -1;
		dlv->escape_dist[i] = -1;
	}
	for (i = 0; i < dlv->playerIDs->len; ++ i)
	{
		struct Monster *player = MTHIID(*(MID*)v_at(dlv->playerIDs, i));
		ylocs[cur_loc] = player->yloc;
		xlocs[cur_loc] = player->xloc;
		++ cur_loc;
	}
	for (cur_back = 0; cur_back < cur_loc; ++ cur_back)
	{
		int y, x, min = MAX_DIST + 5;
		if (dlv->player_dist[map_buffer(ylocs[cur_back], xlocs[cur_back])] >= 0)
			continue;
		for (y = -1; y <= 1; ++ y) for (x = -1; x <= 1; ++ x)
		{
			if (y == 0 && x == 0)
				continue;
			int current = dlv->player_dist[map_buffer(ylocs[cur_back]+y, xlocs[cur_back]+x)];
			if (current < min && current >= 0)
				min = current;
		}
		if (min < MAX_DIST + 5)
		{
			dlv->player_dist[map_buffer(ylocs[cur_back], xlocs[cur_back])] = min + 1;
			dlv->escape_dist[map_buffer(ylocs[cur_back], xlocs[cur_back])] = ESC_FUDGE * (min + 1);
			struct TileDist td = {ylocs[cur_back], xlocs[cur_back], ESC_FUDGE * (min + 1)};
			h_push (tiles, &td);
		}
		else
			dlv->player_dist[map_buffer(ylocs[cur_back], xlocs[cur_back])] = 0;
		if (dlv->player_dist[map_buffer(ylocs[cur_back], xlocs[cur_back])] >= MAX_DIST)
			continue;
		for (y = -1; y <= 1; ++ y) for (x = -1; x <= 1; ++ x)
		{
			if (y == 0 && x == 0)
				continue;
			int current = dlv->player_dist[map_buffer(ylocs[cur_back]+y, xlocs[cur_back]+x)];
			if (current == -1 &&
				ylocs[cur_back]+y+1 < map_graph->h && ylocs[cur_back]+y-1 >= 0 &&
				xlocs[cur_back]+x+1 < map_graph->w && xlocs[cur_back]+x-1 >= 0 &&
				map_passable (cur_dlevel, ylocs[cur_back]+y, xlocs[cur_back]+x))
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
			if (dlv->escape_dist[map_buffer(yloc, xloc)] == -1)
				continue;
			if (dlv->escape_dist[map_buffer(yloc, xloc)] >=
				dlv->escape_dist[map_buffer(t->y, t->x)] - ESC_REM)
				continue;
			dlv->escape_dist[map_buffer(yloc, xloc)] =
				dlv->escape_dist[map_buffer(t->y, t->x)] - ESC_REM;
			struct TileDist t1 = {yloc, xloc, dlv->escape_dist[map_buffer(yloc, xloc)]};
			h_push (tiles, &t1);
		}
		h_pop (tiles, NULL);
	}
}

