/* generate.c */

#include "include/all.h"
#include "include/generate.h"
#include "include/rand.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/map.h"
#include "include/magic.h"
#include "include/graphics.h"
#include "include/vector.h"

#include <stdio.h>
#include <assert.h>
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

#define DIR_UP (-MAP_WIDTH)
#define DIR_DN (MAP_WIDTH)
#define DIR_LF (-1)
#define DIR_RT (1)

#define GETB(a,b) get_buf(a,b,buffer)
int is_in_buf (int i, int dir)
{
	int I = i+dir;
	int x = (i%MAP_WIDTH);
	int X = (I%MAP_WIDTH);

	if (I < 0 || I >= MAP_TILES) return 0;
	return ((X-x)*(X-x) <= 1);
}

int get_buf (int i, int dir, int *buffer)
{
	if (!is_in_buf(i, dir)) return 0;
	return buffer[i+dir];
}

uint32_t upsy, upsx, downsy, downsx;

void get_upstair (uint32_t * yloc, uint32_t * xloc)
{
	*yloc = upsy;
	*xloc = upsx;
}

void get_downstair (uint32_t * yloc, uint32_t * xloc)
{
	*yloc = downsy;
	*xloc = downsx;
}

#define ADD_MAP(c, i) {\
struct map_item_struct *mis = malloc (sizeof(struct map_item_struct));\
memcpy (mis, &(map_items[GETMAPITEMID(c)]), sizeof (struct map_item_struct));\
new_thing (THING_DGN, (i) / MAP_WIDTH, (i) % MAP_WIDTH, mis);\
}

bool check_area (int y, int x, int ys, int xs)
{
	int i, j, k;
    if (y < 0 || y + ys >= MAP_HEIGHT ||
        x < 0 || x + xs >= MAP_WIDTH)
        return false;

	k = xs;
	while (k)
	{
		j = ys;
		while (j)
		{
			i = to_buffer (y+j, x+k);
			if (all_things[i]->len != 0) return false;
			-- j;
		}
		-- k;
	}
    return true;
}

int total_rooms = 0;
bool attempt_room (int y, int x, int ys, int xs)
{
    int i, j, k;
    if (!check_area (y-2, x-2, ys+4, xs+4)) return false;

	k = xs;
	while (k)
	{
		j = ys;
		while (j)
		{
			i = to_buffer (y+j, x+k);
            ADD_MAP (DOT, i);
			-- j;
		}
		-- k;
	}
    ++ total_rooms;
    return true;
}

void add_another_room ()
{
    int i;

    do
        i = RN(MAP_TILES);
    while (all_things[i]->len == 0);

    if (all_things[i+1]->len == 0)
    {
        int x = (i+1)%MAP_WIDTH, y = (i+1)/MAP_WIDTH;
        if (attempt_room (y - 2 - RN(3), x + 1, 6 + RN(3), 6))
        {
            ADD_MAP(DOT, i+1);
            ADD_MAP(DOT, i+2);
        }
    }
    else if (all_things[i-1]->len == 0)
    {
        int x = (i-1)%MAP_WIDTH, y = (i-1)/MAP_WIDTH;
        if (attempt_room (y - 2 - RN(3), x - 8, 6 + RN(3), 6))
        {
            ADD_MAP(DOT, i-1);
            ADD_MAP(DOT, i-2);
        }
    }
    else if (all_things[i-MAP_WIDTH]->len == 0)
    {
        int x = (i-MAP_WIDTH)%MAP_WIDTH, y = (i-MAP_WIDTH)/MAP_WIDTH;
        if (attempt_room (y - 8, x - 3 - RN(5), 6, 8 + RN(5)))
        {
            ADD_MAP(DOT, i-MAP_WIDTH);
            ADD_MAP(DOT, i-MAP_WIDTH*2);
        }
    }
    else if (all_things[i+MAP_WIDTH]->len == 0)
    {
        int x = (i+MAP_WIDTH)%MAP_WIDTH, y = (i+MAP_WIDTH)/MAP_WIDTH;
        if (attempt_room (y + 1, x - 3 - RN(5), 6, 8 + RN(5)))
        {
            ADD_MAP(DOT, i+MAP_WIDTH);
            ADD_MAP(DOT, i+MAP_WIDTH*2);
        }
    }
}

struct Item *gen_item ()
{
	struct item_struct *is = &(items[RN(NUM_ITEMS)]);
	struct Item it = {is, 0, is->wt, NULL};
	if (is->type == IT_JEWEL) it.attr = RN(NUM_JEWELS) << 16;
	struct Item *ret = malloc(sizeof(it));
	memcpy (ret, &it, sizeof(it));
	return ret;
}

void generate_map (enum LEVEL_TYPE type)
{
	int start, end;

	if (type == LEVEL_MINES)
	{
		/* TODO */
	}
	else if (type == LEVEL_NORMAL)
	{
		int i, y, x;

        total_rooms = 0;
		attempt_room (MAP_HEIGHT/2 - 2 - RN(3), MAP_WIDTH/2 - 3 - RN(5), 15, 20);
        do add_another_room ();
        while (total_rooms < 100);

        start = to_buffer (MAP_HEIGHT/2, MAP_WIDTH/2);
		end = mons_gen (1, start);
		
		for (i = 0; i < 100; ++ i)
		{
			do
			{
				y = RN (MAP_HEIGHT);
				x = RN (MAP_WIDTH);
			}
			while (!is_safe_gen (y, x));
			ADD_MAP (DOT, to_buffer (y, x));
			new_thing (THING_ITEM, y, x, gen_item ());
		}

		/* clear space at the beginning (for the up-stair) */
		ADD_MAP (DOT, start);

		/* clear space for the down-stair */
		ADD_MAP (DOT, end);

		/* fill the rest up with walls */
		for (i = 0; i < MAP_TILES; ++i)
			if (all_things[i]->len == 0)
                ADD_MAP (ACS_WALL, i);
	}
	else if (type == LEVEL_MAZE)
	{
		/* TODO */
	}
}

/* can a monster be generated here? (no monsters or walls in the way) */
bool is_safe_gen (uint32_t yloc, uint32_t xloc)
{
	struct Thing *T;
	struct map_item_struct *m;
	int n = to_buffer(yloc, xloc);
	LOOP_THING(n, i)
	{
		T = THING(n, i);
		if (T->type == THING_MONS)
			return false;
		if (T->type == THING_DGN)
		{
			m = T->thing;
			if (!m->attr & 1)
				return false;
		}
	}
	return true;
}

char *real_player_name;
struct Monster *Pl;

/* type:
 * 0 : initialised at start of game
 * 1 : generated at start of level
 * 2 : randomly throughout level */
uint32_t mons_gen (int type, int32_t param)
{
	int32_t luck, start;
	uint32_t end;
	if (type == 0)
	{
		int i;
		for (i = 0; i < MAP_TILES; ++ i)
			v_dinit (all_things[i]);

		start = param;
		upsy = start / MAP_WIDTH;
		upsx = start % MAP_WIDTH;
		struct Monster asdf = {MTYP_HUMAN, 1, 0, 20, 20, 0, 0, {{0},}, {0,}, 0, 0};
		Pl = malloc (sizeof(asdf));
		memcpy (Pl, &asdf, sizeof(asdf));

		real_player_name = malloc(85);
		Pl->name = real_player_name;
		strcpy (Pl->name, "_");
		U.player = new_thing (THING_MONS, upsy, upsx, Pl);
	}
	else if (type == 1)
	{
		/* Up-stair */
		start = param;
		ADD_MAP('<', start);

		/* Down-stair */
		do
			end = RN(MAP_TILES);
		while (end == start);
		ADD_MAP('>', end);

		/* Move to the up-stair */
		thing_bmove (get_player(), start);
		return end;
	}
	else if (type == 2)
	{
		luck = param;
		if (RN(100) >= (15 - 2*luck))
			return 0;

		struct Monster *p = malloc(sizeof(*p));
		memclr (p, sizeof(*p));
		p->type = player_gen_type ();
		p->HP = (mons[p->type].flags >> 28) + (mons[p->type].exp >> 1);
		p->HP += RN(p->HP / 3);
		p->HP_max = p->HP;
		p->name = NULL;
		uint32_t xloc = RN(MAP_WIDTH), yloc = RN(MAP_HEIGHT);
		if (is_safe_gen(yloc, xloc))
			new_thing (THING_MONS, yloc, xloc, p);
		else
			free (p);
	}
	return 0;
}
