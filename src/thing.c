/* thing.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/item.h"
#include "include/monst.h"
#include "include/map.h"
#include "include/generate.h"
#include "include/drawing.h"
#include "include/graphics.h"
#include "include/dlevel.h"
#include "include/event.h"
#include "include/rand.h"

TID curID = 0;
TID getID ()
{
	return (++curID);
}

void thing_watchvec (Vector vec)
{
	int i;
	for (i = 0; i < vec->len; ++ i)
	{
		struct Thing *th = v_at (vec, i);
		THIID (th->ID) = th;
	}
}

void rem_id (TID id)
{
	struct Thing *th = THIID(id);
	struct DLevel *lvl = dlv_lvl (th->dlevel);
	int n = map_buffer (th->yloc, th->xloc);
	v_rptr (lvl->things[n], th);
	thing_watchvec (lvl->things[n]);
	THIID(id) = NULL;
}

void rem_mid (TID id)
{
	struct Monster *th = MTHIID(id);
	struct DLevel *lvl = dlv_lvl (th->dlevel);
	int n = map_buffer (th->yloc, th->xloc);
	MTHIID (id) = NULL; 
	memset (&lvl->mons[n], 0, sizeof(struct Monster));
	return;
}

void monsthing_move (struct Monster *thing, int new_level, int new_y, int new_x)
{
	if (thing->yloc == new_y && thing->xloc == new_x && thing->dlevel == new_level)
		return;

	struct DLevel *olv = dlv_lvl (thing->dlevel),
	              *nlv = dlv_lvl (new_level);

	int old = map_buffer (thing->yloc, thing->xloc),
	    new = map_buffer (new_y, new_x);

	if (nlv->mons[new].ID) panic ("monster already there");
	memcpy (&nlv->mons[new], &olv->mons[old], sizeof(struct Monster));
	memset (&olv->mons[old], 0, sizeof(struct Monster));
	thing = &nlv->mons[new];
	MTHIID (thing->ID) = thing;

	thing->yloc = new_y;
	thing->xloc = new_x;
	thing->dlevel = new_level;
}

void update_item_pointers (Vector vec)
{
	int i;
	for (i = 0; i < vec->len; ++ i)
	{
		struct Item *item = v_at (vec, i);
		ITEMID (item->ID) = item;
	}
}

void rem_itemid (TID ID)
{
	struct Item *item = ITEMID(ID);
	if (!item)
		return;
	int n;
	Vector items;
	if (item->loc.loc == LOC_DLVL)
	{
		n = map_buffer (item->loc.dlvl.yloc, item->loc.dlvl.xloc);
		items = dlv_lvl(item->loc.dlvl.dlevel)->items[n];
	}
	else if (item->loc.loc == LOC_FLIGHT)
	{
		n = map_buffer (item->loc.fl.yloc, item->loc.fl.xloc);
		items = dlv_lvl(item->loc.fl.dlevel)->items[n];
	}
	struct Monster *mons;
	switch (item->loc.loc)
	{
	case LOC_NONE:
		return;
	case LOC_DLVL:
	case LOC_FLIGHT:
		ITEMID(ID) = NULL;
		v_rem (items, ((uintptr_t)item - (uintptr_t)items->data)/sizeof(*item));
		update_item_pointers (items);
		return;
	case LOC_INV:
		ITEMID(item->ID) = NULL;
		pack_rem (MTHIID(item->loc.inv.monsID)->pack, item->loc.inv.invnum);
		return;
	case LOC_WIELDED:
		ITEMID(item->ID) = NULL;
		mons = MTHIID(item->loc.wield.monsID);
		mons->wearing.weaps[item->loc.wield.arm] = NULL;
		pack_rem (mons->pack, item->loc.wield.invnum);
		return;
	}
	panic("End of rem_itemid reached");
}

void item_free (struct Item *item)
{
	rem_itemid (item->ID);
	if (item->name)
		free(item->name);
}

void item_makeID (struct Item *item)
{
	if (!item->ID)
	{
		item->ID = getID();
		v_push (all_ids, &item);
	}
	else
		ITEMID(item->ID) = item;
}

struct Item *instantiate_item (union ItemLoc loc, struct Item *from)
{
	struct Item it;
	struct Item *ret;
	int n;
	Vector items;
	if (loc.loc == LOC_DLVL)
	{
		n = map_buffer (loc.dlvl.yloc, loc.dlvl.xloc);
		items = dlv_lvl(loc.dlvl.dlevel)->items[n];
	}
	else if (loc.loc == LOC_FLIGHT)
	{
		n = map_buffer (loc.fl.yloc, loc.fl.xloc);
		items = dlv_lvl(loc.fl.dlevel)->items[n];
	}
	struct Monster *th;
	switch (loc.loc)
	{
	case LOC_NONE:
		break;
	case LOC_DLVL:
	case LOC_FLIGHT:
		memcpy (&it, from, sizeof(struct Item));
		memcpy (&it.loc, &loc, sizeof(loc));
		ret = v_push (items, &it);
		item_makeID (ret);
		update_item_pointers (items);
		return ret;
	case LOC_INV:
		memcpy (&it, from, sizeof(struct Item));
		memcpy (&it.loc, &loc, sizeof(loc));
		th = MTHIID (loc.inv.monsID);
		if (!pack_add (&th->pack, &it, loc.inv.invnum))
			panic("item already in inventory location in new_item");
		ret = &th->pack->items[loc.inv.invnum];
		item_makeID (ret);
		return ret;
	case LOC_WIELDED:
		memcpy (&it, from, sizeof(struct Item));
		memcpy (&it.loc, &loc, sizeof(loc));
		th = MTHIID (loc.wield.monsID);
		if (!pack_add (&th->pack, &it, loc.wield.invnum))
			panic("item already in inventory location in new_item");
		ret = &th->pack->items[loc.wield.invnum];
		item_makeID (ret);
		if (th->wearing.weaps[loc.wield.arm])
			panic("already wielding an item in new_item");
		th->wearing.weaps[loc.wield.arm] = ret;
		return ret;
	}
	panic("end of new_item reached");
	return 0;
}

struct Item *item_put (struct Item *item, union ItemLoc loc)
{
	struct Item temp;
	memcpy (&temp, item, sizeof(temp));
	rem_itemid (item->ID);
	return instantiate_item (loc, &temp);
}

void thing_free (struct Thing *thing)
{
	if (!thing)
		return;

	switch (thing->type)
	{
		case THING_DGN:
		case THING_NONE:
			break;
	}
}

int TSIZ[] = {
	0,
	sizeof (struct map_item_struct)
};

struct Thing *new_thing (uint32_t type, struct DLevel *lvl, uint32_t y, uint32_t x, void *actual_thing)
{
	int n = map_buffer (y, x);
	struct Thing t = {type, lvl->level, getID(), y, x, {}};

	if (t.ID != all_ids->len)
		panic ("IDs error");

	memcpy (&t.thing, actual_thing, TSIZ[type]);
	struct Thing *ret;
		ret = v_push (lvl->things[n], &t);
		v_push (all_ids, &ret);
		thing_watchvec (lvl->things[n]);

	return ret;
}

struct Monster *new_mons (struct DLevel *lvl, uint32_t y, uint32_t x, void *actual_thing)
{
	int n = map_buffer (y, x);
	struct Monster t;
	memcpy (&t, actual_thing, sizeof(struct Monster));
	t.ID = getID();
	t.dlevel = lvl->level;
	t.yloc = y; t.xloc = x;
	struct Monster *ret = &lvl->mons[n];
	if (ret->ID) panic ("monster already there!");
	memcpy (ret, &t, sizeof(t));
	v_push (all_ids, &ret);
	return ret;
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
#define US(w) (sq_attr[w]?(!sq_cansee[w])*2:1)

void set_can_see (struct Monster *player, uint8_t *sq_cansee, uint8_t *sq_attr,
                  glyph *sq_memory, glyph *sq_vis, glyph *sq_nonvis)
{
	int Yloc = player->yloc, Xloc = player->xloc;
	int Y, X, w;

	/* Anything you could see before you can't necessarily now */
	for (w = 0; w < map_graph->a; ++ w)
		if (sq_cansee[w] == 2)
			sq_cansee[w] = 1;

	/* This puts values on the grid -- whether or not we can see (or have seen) this square */
	// TODO draw more lines (not just starting at player) so that "tile A visible to tile B"
	// is symmetric in A and B; also want that if the player moves between two adjacent squares,
	// then the player can see (from one or other of the squares) anything they plausibly could
	// have seen while moving between them. This doesn't currently hold (exercise); would it
	// hold if we drew every line through the player, not just those starting there?
	// TODO maybe add a range limit
	for (w = 0; w < gr_area; ++ w)
		bres_draw (Yloc, Xloc, sq_cansee, sq_attr, NULL, map_graph->cy + w / gr_w, map_graph->cx + w % gr_w);

	/* Do the drawing */
	struct Monster *monsters = dlv_lvl(player->dlevel)->mons;
	for (Y = 0, w = 0; Y < map_graph->h; ++Y)
	{
		for (X = 0; X < map_graph->w; ++X, ++w)
		{
			struct Monster *mons = &monsters[w];
			if (sq_cansee[w] == 2)
				sq_memory[w] = sq_nonvis[w];

			if (sq_cansee[w] < 2 && sq_memory[w] != ACS_WALL)
			{
				gra_baddch (map_graph, w, sq_memory[w]);
				continue;
			}
			else if (sq_cansee[w] == 2 && mons->ID)
			{
				//if (can see monster TODO )
				gra_bgaddch (map_graph, w, mons->gl);
				map_graph->flags[w] |= 1 |
				                       (1<<12) | ((1+mons->status.moving.ydir)*3 +
				                                  (1+mons->status.moving.xdir)    )<<8 |
				                       (1<<17) | ((1+mons->status.attacking.ydir)*3 +
				                                  (1+mons->status.attacking.xdir)    )<<13;
				continue;
			}
			else if (sq_memory[w] != ACS_WALL)
			{
				gra_baddch (map_graph, w, sq_vis[w]);
				continue;
			}
			else if (!sq_cansee[w])
				continue;

			int h, j, k, l, y, u, b, n;

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

			/* Finally, do the actual drawing of the wall. */
			glyph output = WALL_TYPE (y, u, h, j, k, l, b, n);
			if (sq_cansee[w] == 2)
				output |= COL_TXT_BRIGHT;
			gra_baddch (map_graph, w, output);
		}
	}
}

uint32_t *gr_vis, *gr_novis;
void th_init ()
{
	gr_vis = malloc (sizeof (*gr_vis) * map_graph->a);
	gr_novis = malloc (sizeof (*gr_novis) * map_graph->a);
}

void draw_map (struct Monster *player)
{
	struct DLevel *lvl = cur_dlevel;
	Vector *things = lvl->things;
	uint8_t *sq_seen = lvl->seen,
	        *sq_attr = lvl->attr;
	glyph *sq_unseen = lvl->unseen;

	int i, at;
	for (at = 0; at < map_graph->a; ++ at)
	{
		gr_vis[at] = gr_novis[at] = 0;
		int item_seen = 0;
		sq_attr[at] = 0;
		if (lvl->items[at]->len > 0)
		{
			item_seen = 1;
			gr_vis [at] = gr_novis [at] = ((struct Item *)v_at (lvl->items[at], lvl->items[at]->len-1))->type.gl;
		}
		for (i = 0; i < things[at]->len; ++ i)
		{
			struct Thing *th = THING(things, at, i);
			sq_attr [at] = 1;
			switch (th->type)
			{
			case THING_DGN:
				if (item_seen)
					break;
				struct map_item_struct *m = &th->thing.mis;
				if (m->gl == ACS_BIGDOT)
				{
					gr_vis [at] = ACS_BIGDOT;
					gr_novis [at] = ACS_DOT;
				}
				else
					gr_vis [at] = gr_novis [at] = m->gl;
				sq_attr[at] &= m->attr & 1;
				break;
			case THING_NONE:
				printf ("%d %d %d\n", at, i, th->type);
				getchar ();
				panic ("default reached in draw_map()");
			}
		}
	}
	set_can_see (player, sq_seen, sq_attr, sq_unseen, gr_vis, gr_novis);
}

