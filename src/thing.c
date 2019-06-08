/* thing.c */

#include "include/thing.h"
#include "include/item.h"
#include "include/monst.h"
#include "include/generate.h"
#include "include/drawing.h"
#include "include/graphics.h"
#include "include/dlevel.h"
#include "include/event.h"
#include "include/rand.h"
#include "include/debug.h"
#include "include/vector.h"
#include "include/world.h"

#include <string.h>

void rem_ItemID (V_ItemID v, ItemID ID)
{
	int i;
	for (i = 0; i < v->len; ++ i)
	{
		if (ID == v->data[i])
		{
			v_rem (v, i);
			return;
		}
	}
}

void item_rem (ItemID item)
{
	V_ItemID items;
	switch_loc (item)
	{
	case LOC_NONE:
		return;
	case LOC_DLVL:
		items = dlv_items (dlvl.dlevel, dlvl.zloc, dlvl.yloc, dlvl.xloc);
		if (items)
			rem_ItemID (items, item);
		return;
	case LOC_INV:
		pack_rem (mons_pack (inv.monsID), inv.invnum);
		return;
	case LOC_WIELDED:
		mons_setweap (wield.monsID, wield.arm, 0);
		pack_rem (mons_pack (wield.monsID), wield.invnum);
		return;
	}
	panic("End of item_rem reached");
}

void it_destroy (ItemID item)
{
	item_rem (item);
	it_internal (item)->ID = 0;
	//if (item->name)
	//	free(item->name);
}

// locate an extant item
void it_locate (ItemID item, union ItemLoc loc)
{
	MonsID mons;
	struct Item_internal *ii = it_internal (item);
	ii->loc = loc;
	switch_loc (item)
	{
	case LOC_NONE:
		break;
	case LOC_DLVL:
		;V_ItemID items = dlv_items (dlvl.dlevel, dlvl.zloc, dlvl.yloc, dlvl.xloc);
		if (!items)
		{
			// TODO
			it_destroy (item);
			return;
		}
		v_push (items, &item);
		if (it_sort (item) == ITSORT_TURRET && !it_event (item, compute))
			ev_queue (100, compute, item);
		return;
	case LOC_INV:
		mons = inv.monsID;
		if (!pack_add (mons_pack (mons), item, inv.invnum))
			panic("item already in inventory location in it_locate");
		return;
	case LOC_WIELDED:
		mons = wield.monsID;
		if (!pack_add (mons_pack (mons), item, wield.invnum))
			panic("item already in inventory location in it_locate");
		if (mons_getweap (mons, wield.arm))
			panic("already wielding an item in it_locate");
		mons_setweap (mons, wield.arm, item);
		return;
	}
	panic("end of it_locate reached");
}

// allocate memory for new item
ItemID it_create (struct Item_internal *ii, union ItemLoc loc)
{
	ii = v_push (world.items, ii);
	ItemID item = ii->ID = world.items->len - 1;
	it_locate (item, loc);
	return item;
}

// change location of an extant item
void it_put (ItemID item, union ItemLoc loc)
{
	item_rem (item);
	it_locate (item, loc);
}

void mons_destroy (MonsID mons)
{
	int d, z, y, x;
	mons_getloc (mons, &d, &z, &y, &x);
	dlv_setmons (d, z, y, x, 0);
	mons_internal (mons)->ID = 0;
	return;
}

void mons_move (MonsID mons, int dlevel, int new_z, int new_y, int new_x)
{
	int d, z, y, x;
	mons_getloc (mons, &d, &z, &y, &x);
	if (dlevel == d && new_z == z && new_y == y && new_x == x)
		return;

	if (dlv_mons (dlevel, new_z, new_y, new_x)) panic ("monster already there");
	dlv_setmons (dlevel, new_z, new_y, new_x, mons);
	dlv_setmons (d, z, y, x, 0);

	struct Monster_internal *mi = mons_internal (mons);
	mi->zloc = new_z;
	mi->yloc = new_y;
	mi->xloc = new_x;
	mi->dlevel = dlevel;
	if (mons_isplayer (mons))
	{
		/* re-eval paths to player */
		dlv_fill_player_dist (dlevel);
		/* check what the player can see now */
		update_knowledge (mons);
	}
}

MonsID mons_create (int dlevel, int z, int y, int x, struct Monster_internal *mi)
{
	if (dlv_mons (dlevel, z, y, x))
		panic ("monster already there!");
	mi = v_push (world.mons, mi);
	MonsID mons = mi->ID = world.mons->len - 1;
	//mons_move (mons, dlevel, z, y, x);
	mi->dlevel = dlevel;
	mi->zloc = z; mi->yloc = y; mi->xloc = x;
	mons_stats_changed (mons);
	dlv_setmons (dlevel, z, y, x, mons);
	ev_queue (rn(100), mpoll, mons);
	ev_queue (1, mregen, mons);
	update_knowledge (mons);
	return mons;
}

/* these are in binary order, clockwise from top */
char ACS_ARRAY[17] = {
	ACS_DOT,
	ACS_TSTUB,
	ACS_RSTUB,
	ACS_LLCORNER,
	ACS_BSTUB,
	ACS_VLINE,
	ACS_ULCORNER,
	ACS_LTEE,
	ACS_LSTUB,
	ACS_LRCORNER,
	ACS_HLINE,
	ACS_BTEE,
	ACS_URCORNER,
	ACS_RTEE,
	ACS_TTEE,
	ACS_PLUS,
	' '
};

/* unused */
int wall_output[256] = {
/* 0 */
0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 5,
0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 5,
/* 64 */
8, 9, 8, 9, 10, 11, 10, 11, 8, 9, 8, 9, 10, 11, 10, 11,
12, 13, 12, 13, 14, 15, 14, 15, 12, 13, 12, 13, 14, 15, 14, 13,
8, 9, 8, 9, 10, 11, 10, 11, 8, 9, 8, 9, 10, 11, 10, 11,
12, 13, 12, 13, 14, 15, 14, 15, 12, 13, 12, 13, 10, 11, 10, 9,
/* 128 */
0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 5,
0, 1, 0, 1, 2, 3, 2, 3, 0, 1, 0, 1, 2, 3, 2, 3,
4, 5, 4, 5, 6, 7, 6, 7, 4, 5, 4, 5, 6, 7, 6, 5,
/* 192 */
8, 9, 8, 9, 10, 11, 10, 10, 8, 9, 8, 9, 10, 11, 10, 10,
12, 13, 12, 13, 14, 15, 14, 14, 12, 13, 12, 13, 14, 15, 14, 12,
8, 9, 8, 9, 10, 11, 10, 10, 8, 9, 8, 9, 10, 11, 10, 10,
12, 5, 12, 5, 14, 7, 14, 6, 12, 5, 12, 5, 10, 3, 10, 16
/*256 */
};

/* What this function does is purely cosmetic - given whether or not
 * the squares surrounding are walls or spaces, this function returns what
 * character should be displayed (corner, straight line, tee, etc). */
glyph WALL_TYPE (int y, int u, int h, int j, int k, int l, int b, int n)
{
	int i = ((k==1) + (l==1)*2 + (j==1)*4 + (h==1)*8) &
		(~((h&&y&&u&&l)<<0)) & (~((j&&n&&u&&k)<<1)) & (~((l&&n&&b&&h)<<2)) & (~((k&&y&&b&&j)<<3));
	return ACS_ARRAY[i];
}
/*
void walls_test ()
{
	int i;
	for (i = 0; i < 256; ++ i)
	{
		int Y = (i&128) > 0,
		    H = (i&64) > 0,
		    B = (i&32) > 0,
		    J = (i&16) > 0,
		    N = (i&8) > 0,
		    L = (i&4) > 0,
		    U = (i&2) > 0,
		    K = (i&1) > 0;
		gra_mvaddch (map_graph, 0, 0, Y?'#':' ');
		gra_mvaddch (map_graph, 1, 0, H?'#':' ');
		gra_mvaddch (map_graph, 2, 0, B?'#':' ');
		gra_mvaddch (map_graph, 2, 1, J?'#':' ');
		gra_mvaddch (map_graph, 2, 2, N?'#':' ');
		gra_mvaddch (map_graph, 1, 2, L?'#':' ');
		gra_mvaddch (map_graph, 0, 2, U?'#':' ');
		gra_mvaddch (map_graph, 0, 1, K?'#':' ');
		gra_mvaddch (map_graph, 1, 1, ACS_ARRAY[wall_output[i]]);
		gra_mvprint (map_graph, 3, 0, "Number %d", i);
		gr_getch ();
	}
}*/

// more restrictive; gives less away
//#define US(w) ((!sq_cansee[w])*2 + (!sq_attr[w]))
// better-looking but leaks some info about layout
#define US(w) (has_wall(lvl,w)?1:((lvl->seen[w] == 0 || !lvl->attr[w])*2))

int has_wall (struct DLevel *lvl, int w)
{
	//int i;
	//for (i = 0; i < lvl->things[w]->len; ++ i)
	//	if (((struct Thing *)v_at(lvl->things[w], i))->thing.mis.gl == ACS_WALL)
	//		return 1;
	return 0;
}

glyph fire_glyph (int f)
{
	return 30 |
		COL_BG (7+f>15?15:7+f, f/2>15?15:f/2, f/4-7<0?0:f/4-7>15?15:f/4-7) |
			(f < 8 ? COL_TXT (11+f/2, 3+f/2, 0) :
			(f < 64 ? COL_TXT (16 - f/8, f/7+6, 0) :
			COL_TXT (16-f/8<0?0:16-f/8, 31-f/4<0?0:31-f/4, f/8-8>15?15:f/8-8)));
}

static gflags map_flags;
glyph glyph_to_draw (struct DCell *cell, int looking)
{
	map_flags = 0;
	int nf = cell->num_fires;//dlv_num_fires (dlevel, z, y, x);
	if (looking && /*lvl->seen[w] == 2 &&*/ nf)
		return fire_glyph (nf);
	/* draw walls */
	/*if (lvl->remembered[w] == ACS_WALL && looking)
	{
		int Y = w / map_graph->w, X = w % map_graph->w;
		int h = 0, j = 0, k = 0, l = 0,
			y = 0, u = 0, b = 0, n = 0;

		if (X)
			h = US(w - 1);
		if (Y)
			k = US(w - map_graph->w);
		if (X < map_graph->w - 1)
			l = US(w + 1);
		if (Y < map_graph->h - 1)
			j = US(w + map_graph->w);
		if (X && Y)
			y = US(w - map_graph->w - 1);
		if (X < map_graph->w - 1 && Y)
			u = US(w - map_graph->w + 1);
		if (X && Y < map_graph->h - 1)
			b = US(w + map_graph->w - 1);
		if (X < map_graph->w - 1 && Y < map_graph->h - 1)
			n = US(w + map_graph->w + 1);

		/ * Finally, do the actual drawing of the wall. * /
		glyph output = WALL_TYPE (y, u, h, j, k, l, b, n);
		if (lvl->seen[w] == 2)
			output |= COL_TXT_BRIGHT;
		return output;
	}*/
	/* draw nothing */
	//if (!lvl->seen[w])
	//	return ' ';

	/* draw what you remember */
	//if (lvl->seen[w] == 1 && looking)
	//	return lvl->remembered[w];
	
	/* draw monster */
	DTile t = cell->tile;//dlv_tile (dlevel, z, y, x);
	glyph bg = tile_types[t].gl & COL_BG_MASK;
	MonsID mons = cell->mons;//dlv_mons (dlevel, z, y, x);
	// how draw for 3d?
	if (mons && looking)
	{
		map_flags = 1;
		if (mons_ev (mons, move))
			map_flags |= (1<<12) | ((1+mons_internal(mons)->status.move.ydir)*3 + (1+mons_internal(mons)->status.move.xdir))<<8;
		//	(1<<17) | ((1+mons->status.attacking.ydir)*3 + (1+mons->status.attacking.xdir))<<13;
		return mons_gl (mons) | bg;
	}

	/* draw topmost item in pile */
	int i;
	V_ItemID items = cell->items;//dlv_items (dlevel, z, y, x);
	for (i = 0; i < items->len; ++ i)
	{
		ItemID item = items->data[items->len-1-i];
		if (it_event (item, flight))
			return it_gl (item) | COL_BG (5,5,5);
		if (it_loc (item) == LOC_DLVL)
			return it_gl (item) | bg;
	}

	return tile_types[t].gl;
}

/* Draws player knowledge on to lvl arrays,
 * then renders all that on screen */
void update_knowledge (MonsID player)
{
	/*if (!player)
	{
		int w;
		for (w = 0; w < map_graph->v; ++ w)
			cur_dlevel->seen[w] = 2; // TODO better
		draw_map ();
		return;
	}*/
	//int Y, X, w;
	//int Yloc = player->yloc, Xloc = player->xloc;
	//int w;
	//struct DLevel *lvl = mons_dlv (player);
	/* Anything you could see before you can't necessarily now */
	//for (w = 0; w < lvl->v; ++ w)
	//	if (lvl->seen[w] == 2)
	//		lvl->seen[w] = 1;

	/* This puts values on the grid -- whether or not we can see (or have seen) this square */
	// TODO draw more lines (not just starting at player) so that "tile A visible to tile B"
	// is symmetric in A and B; also want that if the player moves between two adjacent squares,
	// then the player can see (from one or other of the squares) anything they plausibly could
	// have seen while moving between them. This doesn't currently hold (exercise); would it
	// hold if we drew every line through the player, not just those starting there?
	// TODO maybe add a range limit
	/*for (Y = 0, w = 0; Y < lvl->h; ++Y) for (X = 0; X < lvl->w; ++X, ++w)
		bres_draw (Yloc, Xloc, Y, X, lvl->w,
			&lvl->seen[lvl->a * player->zloc], &lvl->attr[lvl->a * player->zloc], NULL);
	for (w = 0; w < lvl->a; ++ w)
		if (lvl->seen[w + lvl->a])
			lvl->seen[w] = 2;*/
	
	// TODO remove
	//for (w = 0; w < lvl->v; ++ w)
	//	lvl->seen[w] = 2;

	/* draw things you can see */
	//for (w = 0; w < lvl->v; ++ w)
	//	if (lvl->seen[w] == 2)
	//		lvl->remembered[w] = glyph_to_draw (lvl, w, 0);

	//draw_map (mons_dlevel (player), player);
}

void draw_map (int dlevel, MonsID player)
{
	//grx_clear (world.map);
	gr_clear ();
	int z, y, x;
	int Z = 0, Y = 0, X = 0;
	z = world.map->cz;
	if (z < 0)
		z = 0;
	for (; z < world.map->t && z < world.map->cz + world.map->ct; ++ z)
		for (y = 0; y < world.map->h; ++ y) for (x = 0; x < world.map->w; ++ x)
	{
		glyph gl = glyph_to_draw (dlv_cell (dlevel, Z+z, Y+y, X+x), 1);
		grx_mvaddch (world.map, z, y, x, gl);
		world.map->flags[z*world.map->A + y*world.map->w + x] = map_flags;
	}
#if 0
	int w;
	for (w = 0; w < lvl->v; ++ w)
		draw_map_buf (lvl, w);
	return;
	uint8_t *array = malloc (sizeof(uint8_t) * (lvl->v));
	int *queue = malloc (sizeof(int) * (lvl->v + 1));
	int i;
	for (i = 0; i < lvl->a; ++ i)
		array[i] = 0;
	queue[0] = mons_index (player);
	array[queue[0]] = 1;
	int cur, head;
	for (cur = 0, head = 1; cur < head; ++ cur)
	{
		w = queue[cur];
		draw_map_buf (lvl, w);
		while (lvl->tiles[w] == DGN_AIR && w/lvl->a < lvl->t-1)
		{
			w += lvl->a;
			draw_map_buf (lvl, w);
		}
		if (lvl->tiles[w] == DGN_AIR)
			continue;
		int y = (w%lvl->a)/lvl->w, x = w%lvl->w;
		int W = w%lvl->a;
		#define ASDF(X) {queue[head++] = (X); array[(X)%lvl->a] = 1;}
		if (lvl->tiles[w] == DGN_AIR)
		{
			int z = w/lvl->a;
			if (z < lvl->t-1)
				ASDF(w+lvl->a);
			continue;
		}
		if (y > 0 && !array[W-lvl->w])
			ASDF(w-lvl->w);
		if (y < lvl->h-1 && !array[W+lvl->w])
			ASDF(w+lvl->w);
		if (x > 0 && !array[W-1])
			ASDF(w-1);
		if (x < lvl->w-1 && !array[W+1])
			ASDF(w+1);
		if (x > 0 && y > 0 && !array[w-lvl->w-1])
			ASDF(w-lvl->w-1);
	}
	free (array);
	free (queue);
#endif
}

