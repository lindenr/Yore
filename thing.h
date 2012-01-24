#ifndef THING_H_INCLUDED
#define THING_H_INCLUDED

#include <stdint.h>
#include <string.h>
#include "list.h"

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

extern void          rem_by_data(void*);
extern struct list_iter *get_iter(void*);
extern struct List   all_things;
extern struct Thing *new_thing(uint32_t, uint32_t, uint32_t, void*);
extern int          *visualise_map(void);
extern struct Thing *find_thing(void *);
extern int           get_thing_type(char);
extern const char   *get_thing_name(struct Thing);
extern struct Thing *get_thing(void *);
struct Thing        *get_player(void);

#endif /* THING_H_INCLUDED */
