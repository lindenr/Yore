/* generate.c */

#include "include/all.h"
#include "include/generate.h"
#include "include/rand.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/map.h"
#include "include/graphics.h"

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
			if (!list_isempty (all_things[i])) return false;
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
    while (list_isempty (all_things[i]));

    if (list_isempty (all_things[i+1]))
    {
        int x = (i+1)%MAP_WIDTH, y = (i+1)/MAP_WIDTH;
        if (attempt_room (y - 2 - RN(3), x + 1, 6 + RN(3), 6))
        {
            ADD_MAP(DOT, i+1);
            ADD_MAP(DOT, i+2);
        }
    }
    else if (list_isempty (all_things[i-1]))
    {
        int x = (i-1)%MAP_WIDTH, y = (i-1)/MAP_WIDTH;
        if (attempt_room (y - 2 - RN(3), x - 8, 6 + RN(3), 6))
        {
            ADD_MAP(DOT, i-1);
            ADD_MAP(DOT, i-2);
        }
    }
    else if (list_isempty (all_things[i-MAP_WIDTH]))
    {
        int x = (i-MAP_WIDTH)%MAP_WIDTH, y = (i-MAP_WIDTH)/MAP_WIDTH;
        if (attempt_room (y - 8, x - 3 - RN(5), 6, 8 + RN(5)))
        {
            ADD_MAP(DOT, i-MAP_WIDTH);
            ADD_MAP(DOT, i-MAP_WIDTH*2);
        }
    }
    else if (list_isempty (all_things[i+MAP_WIDTH]))
    {
        int x = (i+MAP_WIDTH)%MAP_WIDTH, y = (i+MAP_WIDTH)/MAP_WIDTH;
        if (attempt_room (y + 1, x - 3 - RN(5), 6, 8 + RN(5)))
        {
            ADD_MAP(DOT, i+MAP_WIDTH);
            ADD_MAP(DOT, i+MAP_WIDTH*2);
        }
    }
}

void generate_map (enum LEVEL_TYPE type)
{
	int start, end;

	if (type == LEVEL_MINES)
	{
		/*int t = 200;
		
		start = RN(1520) + 79;
		get_player()->yloc = start/80;
		get_player()->xloc = start%80;

		/ clear space at the beginning (for the up-stair) *
		buffer[start] = DOT;

		/ clear space for the down-stair *
		buffer[mons_gen(1, start)] = DOT;

		/ generate some random spaces *
		while (t--)
			buffer[RN(1520) + 79] = DOT;

		t = 800;
		while (t--)
		{
			int buf = RN(1520) + 79;
			if ((buffer[buf] != DOT)
				&& (GETB(buf, DIR_UP) == DOT || GETB(buf, DIR_DN) == DOT ||
					GETB(buf, DIR_LF) == DOT || GETB(buf, DIR_RT) == DOT))
				buffer[buf] = DOT;
			else
				t++;
		}

		/ Will be a wall if it is at edge of screen, or if it is not a space. *
		for (i = 0; i < 1680; ++i)
			if (buffer[i] != DOT || ((i + 1) % 80) <= 1)
				buffer[i] = 'W';

		/ Add everything to the list. *
		for (i = 0; i < 1680; ++i)
		{
			struct map_item_struct *mis =
				malloc(sizeof(struct map_item_struct));
			memcpy(mis, &(map_items[GETMAPITEMID(buffer[i])]),
				   sizeof(struct map_item_struct));
			new_thing(THING_DGN, i / 80, i % 80, mis);
		}*/
	}
	else if (type == LEVEL_NORMAL)
	{
		int i;

        total_rooms = 0;
		attempt_room (MAP_HEIGHT/2 - 2 - RN(3), MAP_WIDTH/2 - 3 - RN(5), 15, 20);
        do add_another_room ();
        while (total_rooms < 50);

        start = to_buffer (MAP_HEIGHT/2, MAP_WIDTH/2);
		end = mons_gen (1, start);

		/* clear space at the beginning (for the up-stair) */
		ADD_MAP (DOT, start);

		/* clear space for the down-stair */
		ADD_MAP (DOT, end);

		/* fill the rest up with walls */
		for (i = 0; i < MAP_TILES; ++i)
			if (list_isempty (all_things[i]))
                ADD_MAP ('W', i);
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
	ITER_THINGS(i, num)
	{
		T = i->data;
		if (T->type == THING_MONS && T->yloc == yloc && T->xloc == xloc)
			return false;
		if (T->type == THING_DGN && T->yloc == yloc && T->xloc == xloc)
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
		{
			all_things[i].beg = &list_beg;
			all_things[i].end = &list_end;
		}
		start = param;
		upsy = start / MAP_WIDTH;
		upsx = start % MAP_WIDTH;
		struct Monster asdf =
			{ MTYP_HUMAN, 1, 0, 20, 20, 0, 0, {{0},}, {0,}, 0, 0 };
		Pl = malloc(sizeof(asdf));
		memcpy(Pl, &asdf, sizeof(asdf));

		real_player_name = malloc(85);
		Pl->name = real_player_name;
		strcpy (Pl->name, "_");
		U.player = new_thing(THING_MONS, upsy, upsx, Pl);
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
		if (RN(5000) >= (15 - 2*luck))
			return 0;

		struct Monster *p = malloc(sizeof(*p));
		memclr(p, sizeof(*p));
		p->type = player_gen_type();
		p->HP = (mons[p->type].flags >> 28) + (mons[p->type].exp >> 1);
		p->HP += RN(p->HP / 3);
		p->HP_max = p->HP;
		p->name = NULL;
		uint32_t xloc = RN(MAP_WIDTH), yloc = RN(MAP_HEIGHT);
		if (is_safe_gen(yloc, xloc))
			new_thing(THING_MONS, yloc, xloc, p);
		else
			free(p);
	}
	return 0;
}
