#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include <stdint.h>
#include <string.h>
#include "include/list.h"
#include "include/graphics.h"

#define get_player() (U.player)
#define get_pmonster() ((struct Monster*)U.player->thing)

/* How ITER_THINGS works: it's a loop through a list, within a loop through the tiles.
 * Since break'ing would only exit one loop (straight into the other) the condition
 * (iter_good(it) == 0) was added -- this ensures that, if the inner loop break's, the
 * outer one will as well. */
#define ITER_THING(it,n)         struct list_iter *it = NULL;                                                        for (it = all_things[n].beg; iter_good(it); next_iter(&it))
#define ITER_THINGS(it,n) int n; struct list_iter *it = NULL; for (n = 0; n < MAP_TILES && iter_good(it) == 0; ++ n) for (it = all_things[n].beg; iter_good(it); next_iter(&it))

enum THING_TYPE
{
	THING_NONE = 0,				/* not used */
	THING_ITEM,					/* an item */
	THING_MONS,					/* a monster */
	THING_DGN,					/* a dungeon feature (wall, floor, trap etc) */
	THING_CURS					/* a (the?) cursor */
};

struct Thing
{
	enum THING_TYPE type;
	uint32_t yloc, xloc;
	void *thing;
};

uint8_t *get_sq_attr(void);
uint8_t *get_sq_seen(void);
void rem_by_data(void *);
void all_things_free(void);
struct Thing *new_thing(uint32_t, uint32_t, uint32_t, void *);
void visualise_map(void);
struct Thing *find_thing(void *);
int get_thing_type(char);
const char *get_thing_name(struct Thing);
struct Thing *get_thing(void *);
struct list_iter *get_iter(void *);
void thing_free(struct Thing *);
void thing_move(struct Thing *, int, int);
void thing_bmove (struct Thing *, int);

extern struct List all_things[];
extern uint8_t sq_seen[MAP_TILES];

#endif /* THING_H_INCLUDED */
