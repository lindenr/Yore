#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include <stdint.h>
#include <string.h>
#include "list.h"

#define get_player() (U.player)
#define get_pmonster() ((struct Monster*)U.player->thing)

enum THING_TYPE
{
	THING_NONE = 0, /* not used */
	THING_ITEM,     /* an item */
	THING_MONS,     /* a monster */
	THING_DGN,      /* a dungeon feature (wall, floor, trap etc) */
	THING_CURS      /* a (the?) cursor */
};

struct Thing
{
    enum THING_TYPE type;
    uint32_t yloc, xloc;
    void *thing;
};

uint8_t             *get_sq_attr(void);
uint8_t             *get_sq_seen(void);
void          rem_by_data(void*);
struct list_iter *get_iter(void*);
struct List   all_things;
struct Thing *new_thing(uint32_t, uint32_t, uint32_t, void*);
int          *visualise_map(void);
struct Thing *find_thing(void *);
int           get_thing_type(char);
const char   *get_thing_name(struct Thing);
struct Thing *get_thing(void *);

#endif /* THING_H_INCLUDED */
