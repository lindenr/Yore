/* thing.c
 * Linden Ralph */

#include <assert.h>
#include "all.h"
#include "thing.h"
#include "item.h"
#include "monst.h"
#include "map.h"
#include "mycurses.h"

struct List all_things = LIST_INIT;

inline struct list_iter *get_iter(void *data)
{
    struct list_iter *i;

    for (i = all_things.beg; iter_good(i); next_iter(&i))
    {
        struct Thing *t = i->data;
        if (t->thing == data)
            return i;
    }
    return NULL;
}

void rem_by_data(void *data)
{
    struct list_iter *i = get_iter(data);
    if (i)
    {
        list_rem(&all_things, i);
        return;
    }
    /* It's "oh dear" if we get to here... */
}

struct Thing *new_thing(uint32_t type, uint32_t y, uint32_t x, void *actual_thing)
{
	struct list_iter* i;
    struct Thing t = {type, y,x, actual_thing};
	struct Thing *thing = malloc(sizeof(struct Thing));
	memcpy(thing, &t, sizeof(struct Thing));
    push_back(&all_things, thing);
    i = all_things.end;
    return ((struct Thing*)(i->data));
}

/* returns an array of 1680 integers (characters with colour) */
/* 0   1   2   3 ...                  -
 * 80  81  82 ...                     |
 * 160 161 ...                        22
 * 240 ...                            |
 * ...                       ...      -
 *  					... 1679
 *  |-------------80-------------|     */
/* The bottom three lines of the 80*25 console are used for HP, stats, time, etc
 * the top is used for pline() */

int* visualise_map ()
{
	int I;
	struct list_iter *i;
    uint32_t *map = malloc(sizeof(uint32_t)*1680);
    uint32_t *type = malloc(sizeof(uint32_t)*1680);
    for (I = 0; I < 1680; ++ I)
    {
		map[I] = ' ';
        type[I] = THING_NONE;
    }

    for(i = all_things.beg; iter_good(i); next_iter(&i))
    {
        struct Thing *T = i->data;
        unsigned at = ((((T->yloc)<<2)+(T->yloc))<<4) + T->xloc;
		struct Thing th = *T;
        bool changed = false;
        assert(at < 1680);
        switch(th.type)
        {
            case THING_MONS:
            {
				struct Monster *m = th.thing;
                changed = true;
				if (m->name[0] == '_')
					map[at] = mons[m->type].col | mons[m->type].ch | COL_TXT_BRIGHT;
                else
					map[at] = mons[m->type].col | mons[m->type].ch;
                break;
            }
            case THING_ITEM:
            {
                if (type[at] != THING_MONS)
                {
					struct Item *t = th.thing;
                    map[at] = items[t->type].col | items[t->type].ch;
                    changed = true;
                }
                break;
            }
            case THING_DGN:
            {
				if (type[at] == THING_NONE)
				{
					struct map_item_struct *m = th.thing;
                    map[at] = (uint32_t)((unsigned char)(m->ch));
					changed = true;
				}
                break;
            }
        }
        if (changed) 
		{
			type[at] = th.type;
		}
    }
	free(type);
    return map;
}

struct Thing *get_thing(void *data)
{
	struct list_iter *i;

	for (i = all_things.beg; iter_good(i); next_iter(&i))
	{
		if (((struct Thing*)(i->data))->thing == data) return i->data;
	}
    /* CRASH! */
	return NULL;
}

/*const char *get_thing_name(struct Thing th)
{
	if (th.type == THING_ITEM)
	{
		const char *ret = items[((struct Item*)(th.thing))->type].name;
		char *cl = malloc((sizeof(char))*(strlen(ret) + 7));
		cl[0] = '\0';
        gram_a(ret, ret);
		if(((struct Item*)(th.thing))->name == NULL)
			return ret;
		strcat(cl, " named ");
		strcat(cl, ret);
		return cl;
	}
	if (th.type == THING_MONS)
	{
		char *ret = mons [((struct Monster*)(th.thing))->type].name;
        gram_a(ret, ret);
		if(((struct Monster*)(th.thing))->name == NULL)
			return ret;
		char *cl = malloc((sizeof(char))*(strlen(ret) + 8 + strlen(((struct Monster*)(th.thing))->name)));
		strcpy(cl, ret);
		strcat(cl, " called ");
		strcat(cl, ((struct Monster*)(th.thing))->name+1);
		return cl;
	}
	if (th.type == THING_DGN)  return ((struct map_item_struct*)(th.thing))->name;
	/ * shouldn't get here * /
	return "bit of floor";
}*/

struct Thing* get_player()
{
    struct list_iter* i;
	for(i = all_things.beg; iter_good(i); next_iter(&i))
    {
        if (((struct Thing*)(i->data))->type == THING_MONS)
        {
            struct Monster* mon = (struct Monster*)(((struct Thing*)(i->data))->thing);
            if(mon->name[0] == '_') /* player */
            {
                return((struct Thing*)(i->data));
            }
        }
    }
    /* no player
     * crash - what else to do? */
#if defined(DEBUGGING)
	debug_log_error(DEBUG_PLAYER_NOT_FOUND, "player not found");
#endif /* DEBUGGING */
    return((struct Thing*)NULL);
}
