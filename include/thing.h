#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include <string.h>
#include "include/vector.h"
#include "include/graphics.h"
#include "include/item.h"
#include "include/monst.h"
#include "include/map.h"

#define player (*(struct Thing **) v_at (all_ids, 1))
#define pmons  (player->thing.mons)

#define LOOP_THING(n,i)  int i;                                     for (i = 0; i < all_things[n]->len; ++ i)
#define LOOP_THINGS(n,i) int i, n; for (n = 0; n < MAP_TILES; ++ n) for (i = 0; i < all_things[n]->len; ++ i)
#define BREAK(n)         {n = MAP_TILES; break;}

#define THIID(id)        (*(struct Thing **) v_at (all_ids, (id)))
#define THING(n,i)       ((struct Thing*)(all_things[n]->data + (i)*sizeof(struct Thing)))

#define get_ref(n,p) ((((uintptr_t)(p)) - ((uintptr_t)all_things[n]->data)) / sizeof(struct Thing))
#define CONTROL_(c) ((KMOD_CTRL << 16) | (c))

enum THING_TYPE
{
	THING_NONE = 0,   /* not used */
	THING_ITEM,       /* an item */
	THING_MONS,       /* a monster */
	THING_DGN,        /* a dungeon feature (wall, floor, trap etc) */
	THING_CURS        /* a (the?) cursor */
};

struct Thing
{
	enum THING_TYPE type;
	int dlevel, ID;
	uint32_t yloc, xloc;
	union
	{
		struct Item item;
		struct Monster mons;
		struct map_item_struct mis;
	}
	thing;
};

void thing_free            (struct Thing *);
void all_things_free       (void);
void rem_ref               (int, int);
void rem_id                (int);

struct Thing *new_thing    (uint32_t, int, uint32_t, uint32_t, void *);

void visualise_map         (void);

int get_thing_type         (char);
const char *get_thing_name (struct Thing);

void thing_move            (struct Thing *, int, int);
void thing_bmove           (struct Thing *, int);

int getID                  ();

extern Vector all_things[];
extern Vector all_mons;
extern Vector all_ids;
extern uint8_t sq_seen[], sq_attr[];

#endif /* THING_H_INCLUDED */
