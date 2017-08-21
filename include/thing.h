#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include "include/vector.h"
#include "include/graphics.h"
#include "include/item.h"
#include "include/monst.h"
#include "include/map.h"

#define LOOP_THING(t,n,i)  int i;                                        for (i = 0; i < (t)[n]->len; ++ i)
#define LOOP_THINGS(t,n,i) int i, n; for (n = 0; n < map_graph->a; ++ n) for (i = 0; i < (t)[n]->len; ++ i)
#define BREAK(n)           {n = map_graph->a; break;}

#define THIID(id)          (*(struct Thing **) v_at (all_ids, (id)))
#define THING(t,n,i)       ((struct Thing*)((t)[n]->data + (i)*sizeof(struct Thing)))

#define CONTROL_(c) ((KMOD_CTRL << 16) | (c))

typedef int TID;

enum THING_TYPE
{
	THING_NONE = 0,   /* not used */
	THING_ITEM,       /* an item */
	THING_MONS,       /* a monster */
	THING_DGN         /* a dungeon feature (wall, floor, trap etc) */
};

struct Thing
{
	enum THING_TYPE type;
	int dlevel;
	TID ID;
	uint32_t yloc, xloc;
	union
	{
		struct Item item;
		struct Monster mons;
		struct map_item_struct mis;
	}
	thing;
};

/* see dlevel.h */
struct DLevel;

void thing_free            (struct Thing *);
void rem_id                (int);

struct Thing *new_thing    (uint32_t, struct DLevel *, uint32_t, uint32_t, void *);

void draw_map              (struct Thing *);

int get_thing_type         (char);
const char *get_thing_name (struct Thing);

void thing_move            (struct Thing *, int, int, int);
void thing_bmove           (struct Thing *, int, int);

TID  getID                  ();

//void projectile            (struct Thing *, char *, int, int);
//int  pr_at                 (struct DLevel *, int, int);

#endif /* THING_H_INCLUDED */

