/* generate.c */

#include "include/all.h"
#include "include/generate.h"
#include "include/rand.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/map.h"
#include "include/graphics.h"

#include <assert.h>
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

#define DIR_UP (-80)
#define DIR_DN (80)
#define DIR_LF (-1)
#define DIR_RT (1)

#define GETB(a,b) get_buf(a,b,buffer)
int is_in_buf (int i, int dir)
{
	int I = i+dir;
	int x = (i%80);
	int X = (I%80);

	if (I < 0 || I >= 1680) return 0;
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

void attempt_room (int y, int x, int ys, int xs, int *buffer)
{
	int i, j, k;

	k = xs;
	while (k)
	{
		-- k;
		j = ys;
		while (j)
		{
			-- j;
			i = 80*(y+j) + (x+k);
			if (buffer[i] != ' ') return;
		}
	}

	y  += 2; x  += 2;
	ys -= 4; xs -= 4;
	k = xs;
	while (k)
	{
		-- k;
		int j = ys;
		while (j)
		{
			-- j;
			i = 80*(y+j) + (x+k);
			if (buffer[i] == ' ')
				buffer[i] = DOT;
			else
				buffer[i] = ' ';
		}
	}
}

int get_wall (int *buffer)
{
	int i;

  retry:
	do
		i = RN(1680)-1;
	while (buffer[i] != ' ');

	if (GETB(i, DIR_UP) == DOT || GETB(i, DIR_DN) == DOT ||
	    GETB(i, DIR_LF) == DOT || GETB(i, DIR_RT) == DOT)
		return i;

	goto retry;
}

int get_room_dir (int i, int *buffer)
{
	if (GETB(i, DIR_UP) == DOT) return DIR_UP;
	if (GETB(i, DIR_DN) == DOT) return DIR_DN;
	if (GETB(i, DIR_LF) == DOT) return DIR_LF;
	if (GETB(i, DIR_RT) == DOT) return DIR_RT;
	return 0;
}

void place_room (int i, int *buffer)
{
	
}

void generate_map (enum LEVEL_TYPE type)
{
	int i, start;
	int buffer[1680];
	for (i = 0; i < 1680; ++i)
		buffer[i] = ' ';

	if (type == LEVEL_MINES)
	{
		int t = 200;
		
		start = RN(1520) + 79;
		get_player()->yloc = start/80;
		get_player()->xloc = start%80;

		/* clear space at the beginning (for the up-stair) */
		buffer[start] = DOT;

		/* clear space for the down-stair */
		buffer[mons_gen(1, start)] = DOT;

		/* generate some random spaces */
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

		/* Will be a wall if it is at edge of screen, or if it is not a space. */
		for (i = 0; i < 1680; ++i)
			if (buffer[i] != DOT || ((i + 1) % 80) <= 1)
				buffer[i] = 'W';

		/* Add everything to the list. */
		for (i = 0; i < 1680; ++i)
		{
			struct map_item_struct *mis =
				malloc(sizeof(struct map_item_struct));
			memcpy(mis, &(map_items[GETMAPITEMID(buffer[i])]),
				   sizeof(struct map_item_struct));
			new_thing(THING_DGN, i / 80, i % 80, mis);
		}
	}
	else if (type == LEVEL_NORMAL)
	{
		int t, i;
		
		/* TODO up- and down-stairs */

		/* clear space at the beginning (for the up-stair) */
		//buffer[start] = DOT;

		/* clear space for the down-stair */
		//buffer[mons_gen(1, start)] = DOT;
		
		//attempt_room (10, 35, 9, 9, buffer);

		//i = get_wall (buffer);
		//buffer[i] = DOT;
		//i -= get_room_dir (i, buffer);
		//place_room (i, buffer);

		/* Add everything to the list. */
		for (i = 0; i < MAP_TILES; ++i)
		{
			struct map_item_struct *mis = malloc (sizeof(struct map_item_struct));
			memcpy (mis, &(map_items[GETMAPITEMID(DOT)]),
				   sizeof (struct map_item_struct));
			new_thing (THING_DGN, i / MAP_WIDTH, i % MAP_WIDTH, mis);
		}
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

/* type: 0 = initialised at start of game, 1 = generated at start of level, 2
   = randomly throughout level */
uint32_t mons_gen (int type, int32_t param)
{
	int32_t luck, start;
	uint32_t end;
	struct map_item_struct *mis, *mise;
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
		Pl->name[0] = '_';
		Pl->name[1] = '\0';
		U.player = new_thing(THING_MONS, upsy, upsx, Pl);
	}
	else if (type == 1)
	{
		start = param;
		upsy = start / MAP_WIDTH;
		upsx = start % MAP_WIDTH;
		mis = malloc(sizeof(struct map_item_struct));
		memcpy (mis, &(map_items[GETMAPITEMID('<')]),
			    sizeof(struct map_item_struct));
		new_thing (THING_DGN, upsy, upsx, mis);
		do
			end = RN(1520) + 79;
		while (end == start);
		downsy = end / 80;
		downsx = end % 80;
		mise = malloc (sizeof(struct map_item_struct));
		memcpy (mise, &(map_items[GETMAPITEMID('>')]),
			    sizeof(struct map_item_struct));
		new_thing (THING_DGN, downsy, downsx, mise);
		return end;
	}
	else if (type == 2)
	{
		luck = param;
		if (RN(500) >= (10 - luck))
			return 0;

		struct Monster *p = malloc(sizeof(*p));
		memclr(p, sizeof(*p));
		p->type = player_gen_type();
		p->HP = (mons[p->type].flags >> 28) + (mons[p->type].exp >> 1);
		p->HP += RN(p->HP / 3);
		p->HP_max = p->HP;
		p->name = NULL;
		uint32_t xloc = RN(75), yloc = RN(15);
		if (is_safe_gen(yloc, xloc))
			new_thing(THING_MONS, yloc, xloc, p);
		else
			free(p);
	}
	return 0;
}
