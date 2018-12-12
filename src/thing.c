/* thing.c */

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
#include "include/debug.h"

TID curID = 0;
TID getID ()
{
	return (++curID);
}

MID curMID = 0;
MID getMID ()
{
	return (++curMID);
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
	int z = th->zloc, y = th->yloc, x = th->xloc;
	int n = dlv_index (lvl, z, y, x);
	v_rptr (lvl->things[n], th);
	thing_watchvec (lvl->things[n]);
	THIID(id) = NULL;
	draw_map_buf (lvl, n);
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
	struct Item *item = it_at(ID);
	if (!item)
		return;
	int n;
	Vector items;
	struct DLevel *lvl = NULL;
	struct Monster *mons;
	switch (item->loc.loc)
	{
	case LOC_NONE:
		return;
	case LOC_DLVL:
		lvl = dlv_lvl(item->loc.dlvl.dlevel);
		n = dlv_index (lvl, item->loc.dlvl.zloc, item->loc.dlvl.yloc, item->loc.dlvl.xloc);
		items = lvl->items[n];
		goto end_remove;
	case LOC_FLIGHT:
		lvl = dlv_lvl(item->loc.fl.dlevel);
		n = dlv_index (lvl, item->loc.fl.zloc, item->loc.fl.yloc, item->loc.fl.xloc);
		items = lvl->items[n];
		goto end_remove;
	end_remove:
		ITEMID(ID) = NULL;
		v_rptr (items, item);
		update_item_pointers (items);
		draw_map_buf (lvl, n);
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
	//if (item->name)
	//	free(item->name);
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
	struct DLevel *lvl = NULL;
	struct Monster *th;
	switch (loc.loc)
	{
	case LOC_NONE:
		break;
	case LOC_DLVL:
		lvl = dlv_lvl(loc.dlvl.dlevel);
		n = dlv_index (lvl, loc.dlvl.zloc, loc.dlvl.yloc, loc.dlvl.xloc);
		items = lvl->items[n];
		goto end_inst;
	case LOC_FLIGHT:
		lvl = dlv_lvl(loc.fl.dlevel);
		n = dlv_index (lvl, loc.fl.zloc, loc.fl.yloc, loc.fl.xloc);
		items = lvl->items[n];
		goto end_inst;
	end_inst:
		memcpy (&it, from, sizeof(struct Item));
		memcpy (&it.loc, &loc, sizeof(loc));
		ret = v_push (items, &it);
		item_makeID (ret);
		update_item_pointers (items);
		draw_map_buf (lvl, n);
		return ret;
	case LOC_INV:
		memcpy (&it, from, sizeof(struct Item));
		memcpy (&it.loc, &loc, sizeof(loc));
		th = MTHIID (loc.inv.monsID);
		if (!pack_add (&th->pack, &it, loc.inv.invnum))
			panic("item already in inventory location in instantiate_item");
		ret = &th->pack->items[loc.inv.invnum];
		item_makeID (ret);
		return ret;
	case LOC_WIELDED:
		memcpy (&it, from, sizeof(struct Item));
		memcpy (&it.loc, &loc, sizeof(loc));
		th = MTHIID (loc.wield.monsID);
		if (!pack_add (&th->pack, &it, loc.wield.invnum))
			panic("item already in inventory location in instantiate_item");
		ret = &th->pack->items[loc.wield.invnum];
		item_makeID (ret);
		if (th->wearing.weaps[loc.wield.arm])
			panic("already wielding an item in instantiate_item");
		th->wearing.weaps[loc.wield.arm] = ret;
		return ret;
	}
	panic("end of instantiate_item reached");
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

struct Thing *new_thing (enum THING_TYPE type, struct DLevel *lvl,
	int z, int y, int x, void *actual_thing)
{
	int n = dlv_index (lvl, z, y, x);
	struct Thing t = {type, lvl->level, getID(), z, y, x, {}};

	if (t.ID != all_ids->len)
		panic ("IDs error");

	memcpy (&t.thing, actual_thing, TSIZ[type]);
	struct Thing *ret;
	ret = v_push (lvl->things[n], &t);
	v_push (all_ids, &ret);
	thing_watchvec (lvl->things[n]);
	lvl->attr[n] = ret->thing.mis.attr & 1;
	draw_map_buf (lvl, n);

	return ret;
}

void rem_mid (MID id)
{
	struct Monster *th = MTHIID(id);
	struct DLevel *lvl = dlv_lvl (th->dlevel);
	int n = dlv_index (lvl, th->zloc, th->yloc, th->xloc);
	lvl->monsIDs[n] = 0;
	memset (th, 0, sizeof(struct Monster));
	draw_map_buf (lvl, n);
	return;
}

struct Item *it_at (TID id)
{
	if (id <= 0)
		return NULL;
	struct Item *it = *(struct Item **) v_at (all_ids, id);
	if (it && it->ID == id)
		return it;
	return NULL;
}

struct Monster *MTHIID (MID id)
{
	if (id <= 0)
		return NULL;
	struct Monster *ret = v_at (cur_dlevel->mons, id);
	if (!ret->ID)
		return NULL;
	return ret;
}

void mons_move (struct Monster *thing, int new_level, int new_z, int new_y, int new_x)
{
	if (thing->zloc == new_z && thing->yloc == new_y &&
		thing->xloc == new_x && thing->dlevel == new_level)
		return;

	struct DLevel *olv = dlv_lvl (thing->dlevel),
	              *nlv = dlv_lvl (new_level);

	int old = dlv_index (olv, thing->zloc, thing->yloc, thing->xloc),
	    new = dlv_index (nlv, new_z, new_y, new_x);

	if (nlv->monsIDs[new]) panic ("monster already there");
	nlv->monsIDs[new] = olv->monsIDs[old];
	olv->monsIDs[old] = 0;

	thing->zloc = new_z;
	thing->yloc = new_y;
	thing->xloc = new_x;
	thing->dlevel = new_level;
	draw_map_buf (olv, old);
	draw_map_buf (nlv, new);
	if (mons_isplayer (thing))
	{
		/* re-eval paths to player */
		dlv_fill_player_dist (cur_dlevel);
		/* check what the player can see now */
		update_knowledge (thing);
	}
}

struct Monster *new_mons (struct DLevel *lvl, int z, int y, int x, void *actual_thing)
{
	int n = dlv_index (lvl, z, y, x);
	if (n == -1)
		panic ("placement out of bounds");
	struct Monster t;
	memcpy (&t, actual_thing, sizeof(struct Monster));
	t.ID = getMID();
	t.dlevel = lvl->level;
	t.zloc = z; t.yloc = y; t.xloc = x;
	mons_stats_changed (&t);
	if (lvl->monsIDs[n]) panic ("monster already there!");
	lvl->monsIDs[n] = t.ID;
	struct Monster *ret = v_push (lvl->mons, &t);
	draw_map_buf (lvl, n);
	ev_queue (1000 + rn(1000), (union Event) { .mpoll = {EV_MPOLL, ret->ID}});
	ev_queue (1, (union Event) { .mregen = {EV_MREGEN, ret->ID}});
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
#define US(w) (has_wall(lvl,w)?1:((lvl->seen[w] == 0 || !lvl->attr[w])*2))

int has_wall (struct DLevel *lvl, int w)
{
	int i;
	for (i = 0; i < lvl->things[w]->len; ++ i)
		if (((struct Thing *)v_at(lvl->things[w], i))->thing.mis.gl == ACS_WALL)
			return 1;
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
glyph glyph_to_draw (struct DLevel *lvl, int w, int looking)
{
	map_flags = 0;
	if (looking && lvl->seen[w] == 2 && lvl->num_fires[w])
		return fire_glyph (lvl->num_fires[w]);
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
	if (!lvl->seen[w])
		return ' ';

	/* draw what you remember */
	if (lvl->seen[w] == 1 && looking)
		return lvl->remembered[w];
	
	/* draw monster */
	struct Monster *mons = MTHIID(lvl->monsIDs[w]);
	if (mons && looking)
	{
		map_flags = 1 |
			(1<<12) | ((1+mons->status.moving.ydir)*3 + (1+mons->status.moving.xdir))<<8 |
			(1<<17) | ((1+mons->status.attacking.ydir)*3 + (1+mons->status.attacking.xdir))<<13;
		return mons->gl;
	}

	/* draw topmost item in pile */
	int i;
	for (i = 0; i < lvl->items[w]->len; ++ i)
	{
		struct Item *item = v_at (lvl->items[w], lvl->items[w]->len-1-i);
		if (item->loc.loc == LOC_DLVL)
			return it_gl (item);
		else if (item->loc.loc == LOC_FLIGHT && looking)
			return it_gl (item) | COL_BG (5,5,5);
	}

	/* draw topmost dungeon feature */
	i = lvl->things[w]->len - 1;
	if (i >= 0)
	{
		struct Thing *th = v_at (lvl->things[w], i);
		switch (th->type)
		{
		case THING_DGN: ;
			struct map_item_struct *m = &th->thing.mis;
			//int dist = lvl->escape_dist[w] + (5*lvl->player_dist[w])/3;
			//if (looking && m->gl == ACS_BIGDOT && dist >= 0)
			//	return '0' + dist%10;
			if (looking || !(m->gl == ACS_BIGDOT || m->gl == ACS_CORRIDOR))
				return m->gl
					//| ((dist==-1)?0:COL_BG (
					//	dist < 30 ? 15 - dist/2 : 0, dist<14 ? 14-dist : 0, dist<20?8:(50-dist)/4))
					;
			else if (m->gl == ACS_BIGDOT)
				return ACS_DOT;
			else
				return ACS_DIMCORRIDOR;
		case THING_NONE:
			printf ("%d %d %d\n", w, i, th->type);
			panic ("THING_NONE type exists in glyph_to_draw");
			break;
		}
	}
	return 0;
}

/* Draws player knowledge on to lvl arrays,
 * then renders all that on screen */
void update_knowledge (struct Monster *player)
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
	int w;
	struct DLevel *lvl = dlv_lvl (player->dlevel);
	/* Anything you could see before you can't necessarily now */
	for (w = 0; w < lvl->v; ++ w)
		if (lvl->seen[w] == 2)
			lvl->seen[w] = 1;

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
	for (w = 0; w < lvl->v; ++ w)
		lvl->seen[w] = 2;

	/* draw things you can see */
	for (w = 0; w < lvl->v; ++ w)
		if (lvl->seen[w] == 2)
			lvl->remembered[w] = glyph_to_draw (lvl, w, 0);

	draw_map (lvl, player);
}

void th_init ()
{
}

extern Graph map_graph;
void draw_map (struct DLevel *lvl, struct Monster *player)
{
	grx_clear (map_graph);
	int w;
	for (w = 0; w < lvl->v; ++ w)
		draw_map_buf (lvl, w);
	return;
	uint8_t *array = malloc (sizeof(uint8_t) * (lvl->v));
	int *queue = malloc (sizeof(int) * (lvl->v + 1));
	int i;
	for (i = 0; i < lvl->a; ++ i)
		array[i] = 0;
	queue[0] = dlv_index (lvl, player->zloc, player->yloc, player->xloc);
	array[queue[0]] = 1;
	int cur, head;
	for (cur = 0, head = 1; cur < head; ++ cur)
	{
		w = queue[cur];
		draw_map_buf (lvl, w);
		while (((struct Thing *) v_at (lvl->things[w], 0))->thing.mis.gl == ' ' && w/lvl->a < lvl->t-1)
		{
			w += lvl->a;
			draw_map_buf (lvl, w);
		}
		if (((struct Thing *) v_at (lvl->things[w], 0))->thing.mis.gl == 0)
			continue;
		int y = (w%lvl->a)/lvl->w, x = w%lvl->w;
		int W = w%lvl->a;
		#define ASDF(X) {queue[head++] = (X); array[(X)%lvl->a] = 1;}
		if (((struct Thing *) v_at (lvl->things[w], 0))->thing.mis.gl == ' ')
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
}

void draw_map_xyz (struct DLevel *lvl, int z, int y, int x)
{
	int w = dlv_index (lvl, z, y, x);
	draw_map_buf (lvl, w);
}

void draw_map_buf (struct DLevel *lvl, int w)
{
	glyph gl = glyph_to_draw (lvl, w, 1);
	gra_baddch (map_graph, w, gl);
	map_graph->flags[w] |= map_flags;
}

