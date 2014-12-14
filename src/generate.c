/* generate.c */

#include "include/all.h"
#include "include/generate.h"
#include "include/rand.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/map.h"
#include "include/graphics.h"
#include "include/vector.h"
#include "include/dlevel.h"

#include <stdio.h>
#include <assert.h>

#define ADD_MAP(c, i) new_thing (THING_DGN, lvl, (i) / map_graph->w, (i) % map_graph->w, &map_items[GETMAPITEMID(c)])

bool check_area (struct DLevel *lvl, int y, int x, int ys, int xs)
{
	Vector *things = lvl->things;
	int i, j, k;
	if (y < 0 || y + ys >= map_graph->h ||
		x < 0 || x + xs >= map_graph->w)
		return false;

	k = xs;
	while (k)
	{
		j = ys;
		while (j)
		{
			i = map_buffer (y+j, x+k);
			if (things[i]->len != 0) return false;
			-- j;
		}
		-- k;
	}
	return true;
}

int total_rooms = 0;
bool attempt_room (struct DLevel *lvl, int y, int x, int ys, int xs)
{
	int i, j, k;
	if (!check_area (lvl, y-2, x-2, ys+4, xs+4)) return false;

	k = xs;
	while (k)
	{
		j = ys;
		while (j)
		{
			i = map_buffer (y+j, x+k);
			ADD_MAP (DOT, i);
			-- j;
		}
		-- k;
	}
	++ total_rooms;
	return true;
}

void add_another_room (struct DLevel *lvl)
{
	Vector *things = lvl->things;
	int i;

	do
		i = rn(map_graph->a);
	while (things[i]->len == 0);

	if (things[i+1]->len == 0)
	{
		int x = (i+1)%map_graph->w, y = (i+1)/map_graph->w;
		if (attempt_room (lvl, y - 2 - rn(3), x + 1, 6 + rn(3), 6))
		{
			ADD_MAP(DOT, i+1);
			ADD_MAP(DOT, i+2);
		}
	}
	else if (things[i-1]->len == 0)
	{
		int x = (i-1)%map_graph->w, y = (i-1)/map_graph->w;
		if (attempt_room (lvl, y - 2 - rn(3), x - 8, 6 + rn(3), 6))
		{
			ADD_MAP(DOT, i-1);
			ADD_MAP(DOT, i-2);
		}
	}
	else if (things[i-map_graph->w]->len == 0)
	{
		int x = (i-map_graph->w)%map_graph->w, y = (i-map_graph->w)/map_graph->w;
		if (attempt_room (lvl, y - 8, x - 3 - rn(5), 6, 8 + rn(5)))
		{
			ADD_MAP(DOT, i-map_graph->w);
			ADD_MAP(DOT, i-map_graph->w*2);
		}
	}
	else if (things[i+map_graph->w]->len == 0)
	{
		int x = (i+map_graph->w)%map_graph->w, y = (i+map_graph->w)/map_graph->w;
		if (attempt_room (lvl, y + 1, x - 3 - rn(5), 6, 8 + rn(5)))
		{
			ADD_MAP(DOT, i+map_graph->w);
			ADD_MAP(DOT, i+map_graph->w*2);
		}
	}
}

struct Item *gen_item ()
{
	ityp is;
	memcpy (&is, &(items[rn(NUM_ITEMS)]), sizeof(is));
	struct Item it = {is, 0, is.wt, NULL};
	//if (is.type == IT_JEWEL)
	//	it.attr |= rn(NUM_JEWELS) << 16;
	struct Item *ret = malloc(sizeof(it));
	memcpy (ret, &it, sizeof(it));
	return ret;
}

void generate_map (struct DLevel *lvl, enum LEVEL_TYPE type)
{
	int start, end;
	Vector *things = lvl->things;

	if (type == LEVEL_MINES)
	{
		/* TODO */
	}
	else if (type == LEVEL_NORMAL)
	{
		int i, y, x;

		total_rooms = 0;
		attempt_room (lvl, map_graph->h/2 - 2 - rn(3), map_graph->w/2 - 3 - rn(5), 15, 20);
		do add_another_room (lvl);
		while (total_rooms < 100);

		start = map_buffer (map_graph->h/2, map_graph->w/2);
		end = mons_gen (lvl, 1, start);
		
		for (i = 0; i < 100; ++ i)
		{
			do
			{
				y = rn (map_graph->h);
				x = rn (map_graph->w);
			}
			while (!is_safe_gen (lvl, y, x));
			ADD_MAP (DOT, map_buffer (y, x));

			struct Item *item = gen_item ();
			new_thing (THING_ITEM, lvl, y, x, item);
			free (item);
		}

		/* clear space at the beginning (for the up-stair) */
		ADD_MAP (DOT, start);

		/* clear space for the down-stair */
		ADD_MAP (DOT, end);

		/* fill the rest up with walls */
		for (i = 0; i < map_graph->a; ++i)
			if (things[i]->len == 0)
				ADD_MAP (ACS_WALL, i);
	}
	else if (type == LEVEL_MAZE)
	{
		/* TODO */
	}
}

/* can a monster be generated here? (no monsters or walls in the way) */
bool is_safe_gen (struct DLevel *lvl, uint32_t yloc, uint32_t xloc)
{
	Vector *things = lvl->things;
	struct Thing *T;
	struct map_item_struct *m;
	int n = map_buffer(yloc, xloc);
	LOOP_THING(things, n, i)
	{
		T = THING(things, n, i);
		if (T->type == THING_MONS)
			return false;
		if (T->type == THING_DGN)
		{
			m = &(T->thing.mis);
			if (!m->attr & 1)
				return false;
		}
	}
	return true;
}

char *real_player_name;

/* type:
 * 0 : initialised at start of game
 * 1 : generated at start of level
 * 2 : randomly throughout level */
uint32_t mons_gen (struct DLevel *lvl, int type, int32_t param)
{
	int32_t luck, start;
	int32_t end;
	uint32_t upsy, upsx;
	if (type == 0)
	{
		start = param;
		upsy = start / map_graph->w;
		upsx = start % map_graph->w;
		struct Monster asdf = {MTYP_HUMAN, 1, 0, 20, 20, 0, 0, {{0},}, {0,}, 0, 0};
		asdf.name = malloc (85);
		strcpy (asdf.name, "_");
		real_player_name = asdf.name;
		new_thing (THING_MONS, lvl, upsy, upsx, &asdf);
	}
	else if (type == 1)
	{
		/* Up-stair */
		start = param;
		ADD_MAP('<', start);

		/* Down-stair */
		do
			end = (int32_t) rn(map_graph->a);
		while (end == start);
		ADD_MAP('>', end);

		/* Move to the up-stair */
//		thing_bmove (player, start);
		return end;
	}
	else if (type == 2)
	{
		luck = param;
		if (rn(100) >= (uint32_t) (15 - 2*luck))
			return 0;

		struct Monster p;
		memclr (&p, sizeof(p));
		p.type = player_gen_type ();
		p.HP = (mons[p.type].flags >> 28) + (mons[p.type].exp >> 1);
		p.HP += rn(p.HP / 3);
		p.HP_max = p.HP;
		p.name = NULL;
		uint32_t xloc = rn(map_graph->w), yloc = rn(map_graph->h);
		if (is_safe_gen (lvl, yloc, xloc))
			new_thing (THING_MONS, lvl, yloc, xloc, &p);
	}
	return 0;
}

