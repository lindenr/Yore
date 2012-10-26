/* thing.c */

#include <assert.h>
#include "include/all.h"
#include "include/thing.h"
#include "include/item.h"
#include "include/monst.h"
#include "include/map.h"
#include "include/mycurses.h"
#include "include/generate.h"
#include "include/vision.h"
#include "include/output.h"
#include "include/graphics.h"

struct List all_things[MAP_HEIGHT*MAP_WIDTH];

/* At any given point: 0 if we can't see past that square (e.g. wall); 1 if we 
   can remember it (e.g. sword); and 2 if we can't remember its position (e.g. 
   monster). The options are mutually exclusive. */
uint8_t sq_attr[MAP_TILES] = { 0, };

/* Output of the bresenham algorithm (vision.c): 0 if we can't see it (outside 
   our field of vision); 1 if we remember it; and 2 if we are looking at it.
   The options are again mutually exclusive. */
uint8_t sq_seen[MAP_TILES] = { 0, };

/* What to see instead if it turns out that we can't remember something */
uint32_t sq_unseen[MAP_TILES] = { 0, };

/* This is called by the AI to find out what the monsters can see. */
uint8_t *get_sq_attr()
{
	return sq_attr;
}

/* This is very ugly and inelegant. A better/neater solution would be very
   welcome. What this function does is purely cosmetic - given whether or not
   the squares surrounding are walls or spaces, this function returns what
   character should be displayed (corner, straight line, tee, etc). The nested 
   if's make my eyes hurt, but I can't think of a simpler alternative. */
uint32_t WALL_TYPE(uint32_t y, uint32_t u, uint32_t h, uint32_t j, uint32_t k,
				   uint32_t l, uint32_t b, uint32_t n)
{
	int H = (h == DOT || h == ' '),
		J = (j == DOT || j == ' '),
		K = (k == DOT || k == ' '), L = (l == DOT || l == ' ');
	if (H)
	{
		if (K)
		{
			if (L)
			{
				if (J)
					return ACS_HLINE;
				else
					return ACS_VLINE;
			}
			else
			{
				if (J)
					return ACS_HLINE;
				else
					return ACS_ULCORNER;
			}
		}
		else
		{
			if (L)
				return ACS_VLINE;
			else
			{
				if (J)
					return ACS_LLCORNER;
				else
					return ACS_LTEE;
			}
		}
	}
	else
	{
		if (K)
		{
			if (L)
			{
				if (J)
					return ACS_HLINE;
				else
					return ACS_URCORNER;
			}
			else
			{
				if (J)
					return ACS_HLINE;
				else
					return ACS_TTEE;
			}
		}
		else
		{
			if (L)
			{
				if (J)
					return ACS_LRCORNER;
				else
					return ACS_RTEE;
			}
			else
			{
				if (J)
					return ACS_BTEE;
				else
				{
					if (y == DOT || u == DOT || b == DOT || n == DOT)
						return ACS_PLUS;
					else
						return ' ';
				}
			}
		}
	}
}

#define US(w) (sq_seen[w]?(sq_attr[w]?DOT:'W'):DOT)

inline void set_can_see(uint32_t * unseen)
{
	glyph *us = gr_map;
	int Yloc = get_player()->yloc, Xloc = get_player()->xloc;
	int Y, X, w;

	/* Initialise the bres thing */
	bres_start(Yloc, Xloc, sq_seen, sq_attr);

	/* Anything you could see before you can't necessarily now */
	for (w = 0; w < 1680; ++w)
		if (sq_seen[w] == 2)
			sq_seen[w] = 1;

	/* This puts values on the grid -- whether or not we can see (or have
	   seen) this square */
	for (Y = 0; Y < 21; ++Y)
		for (X = 0; X < 80; ++X)
			//bres_draw(Y, X);
			sq_seen[80*Y + X] = 2;

	/* Make everything we can't see dark */
	for (w = 0; w < 1680; ++w)
		if (!sq_seen[w])
			us[w] = ' ';

	/* Do the drawing */
	for (Y = 0, w = 0; Y < 21; ++Y)
	{
		for (X = 0; X < 80; ++X, ++w)
		{
			uint32_t y = DOT, u = DOT, h = DOT, j = DOT, k = DOT, l = DOT, b =
				DOT, n = DOT;

			if (sq_seen[w] == 2 && sq_attr[w] == 0 && us[w] != ' ')
				us[w] |= COL_TXT_BRIGHT;	/* Brighten what you can see iff it's a wall. */

			if (sq_seen[w] == 1 && sq_attr[w] == 2)
				us[w] = unseen[w];	/* Replace something unseeable with what's 
									   behind it. */

			if (sq_attr[w] != 0 || us[w] == ' ')
				continue;		/* Only keep going if it is a wall. */

			/* Again, not especially neat, but I don't think there is much I
			   can do */
			if (X)
				h = US(w - 1);
			if (Y)
				k = US(w - 80);
			if (X < 79)
				l = US(w + 1);
			if (Y < 20)
				j = US(w + 80);
			if (X && Y)
				y = US(w - 81);
			if (X < 79 && Y)
				u = US(w - 79);
			if (X && Y < 20)
				b = US(w + 79);
			if (X < 79 && Y < 20)
				n = US(w + 81);

			/* Finally, do the actual drawing of the wall. */
			if (us[w] & COL_TXT_BRIGHT)
				us[w] = WALL_TYPE(y, u, h, j, k, l, b, n) | COL_TXT_BRIGHT;
			else
				us[w] = WALL_TYPE(y, u, h, j, k, l, b, n);
		}
	}
}

void thing_move (struct Thing *thing, int new_y, int new_x)
{
	int num = to_buffer(thing->yloc, thing->xloc);
	ITER_THING(li, num)
	{
		if (li->data == thing)
			break;
	}
	if (!iter_good(li)) return; /* Couldn't find thing in list */
	list_rem (&all_things[num], li);
	thing->yloc = new_y;
	thing->xloc = new_x;
	num = to_buffer(thing->yloc, thing->xloc);
	push_back (&all_things[num], thing);
}

inline struct list_iter *get_iter(void *data)
{
	ITER_THINGS(i, num)
	{
		struct Thing *t = i->data;
		if (t->thing == data)
			return i;
	}
	return NULL;
}

void thing_free(struct Thing *thing)
{
	if (!thing)
		return;

	switch (thing->type)
	{
		case THING_ITEM:
		{
			struct Item *i = thing->thing;
			if (i->name)
				free(i->name);
			break;
		}
		case THING_MONS:
		{
			struct Monster *monst = thing->thing;
			if (monst->name && monst->name[0])
				free(monst->name);
			if (monst->eating)
				free(monst->eating);
			break;
		}
		default:
			break;
	}
	free(thing->thing);
	free(thing);
}

void rem_by_data(void *data)
{
	ITER_THINGS(i, num)
	{
		struct Thing *t = i->data;
		if (t->thing == data)
			break;
	}
	if (!iter_good(i))
	{
		printf("asdfasdf");
		return;					/* fail */
	}

	list_rem(&all_things[num], i);
	free(i);
}

struct Thing *new_thing(uint32_t type, uint32_t y, uint32_t x, void *actual_thing)
{
	struct Thing t = { type, y, x, actual_thing };
	struct Thing *thing = malloc(sizeof(struct Thing));
	memcpy(thing, &t, sizeof(struct Thing));
	push_back(&all_things[to_buffer(y, x)], thing);
	return thing;
}

/* Directly modifies gr_map[] */
void visualise_map()
{
	uint32_t type[MAP_TILES] = {0,};
	ITER_THINGS(i, at)
	{
		struct Thing *T = i->data;
		struct Thing th = *T;
		bool changed = false;
		switch (th.type)
		{
			case THING_MONS:
			{
				struct Monster *m = th.thing;
				changed = true;
				gr_baddch(at, mons[m->type].col | mons[m->type].ch);
				if (m->name)
					if (IS_PLAYER(m))
					{
						gr_map[at] |= COL_TXT_BRIGHT;
					}
				sq_attr[at] = 2;
				break;
			}
			case THING_ITEM:
			{
				printf("normal %d\n", th.thing);
				if (type[at] != THING_MONS)
				{
					struct Item *t = th.thing;
					gr_baddch(at, t->type->col | t->type->ch);
					changed = true;
				}
				sq_unseen[at] = gr_map[at];
				sq_attr[at] = 1;
				break;
			}
			case THING_DGN:
			{
				if (type[at] == THING_NONE)
				{
					struct map_item_struct *m = th.thing;
					gr_baddch(at, (glyph) ((unsigned char)(m->ch)));
					sq_attr[at] = m->attr & 1;
					changed = true;
				}
				sq_unseen[at] = gr_map[at];
				break;
			}
			default:
			{
				panic("default reached in visualise_map()");
			}
		}
		if (changed)
		{
			type[at] = th.type;
		}
	}
	//set_can_see(sq_unseen);
	gr_refresh();
}

struct Thing *get_thing(void *data)
{
	ITER_THINGS(i, num)
	{
		if (((struct Thing *)(i->data))->thing == data)
			return i->data;
	}
	/* CRASH! */
	return NULL;
}

void all_things_free()
{
	ITER_THINGS(i, num)
		thing_free(i->data);
}
