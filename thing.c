/* thing.c
 * Linden Ralph */

#include <assert.h>
#include "all.h"
#include "thing.h"
#include "item.h"
#include "monst.h"
#include "map.h"
#include "mycurses.h"
#include "generate.h"

struct List all_things = LIST_INIT;
uint8_t sq_attr[1680] = {0,};
uint8_t sq_seen[1680] = {0,};

uint8_t *get_sq_attr() {return sq_attr;}
uint8_t *get_sq_seen() {return sq_seen;}

uint32_t WALL_TYPE(uint32_t y, uint32_t u, uint32_t h, uint32_t j, uint32_t k, uint32_t l, uint32_t b, uint32_t n)
{
    if (h == DOT || h == ' ')
    {
        if (k == DOT || k == ' ')
        {
            if (l == DOT || l == ' ')
            {
                if (j == DOT || j == ' ') return ACS_HLINE;
                else          return ACS_VLINE;
            }
            else
            {
                if (j == DOT || j == ' ') return ACS_HLINE;
                else                      return ACS_ULCORNER;
            }
        }
        else
        {
            if (l == DOT || l == ' ')     return ACS_VLINE;
            else
            {
                if (j == DOT || j == ' ') return ACS_LLCORNER;
                else          return ACS_LTEE;
            }
        }
    }
    else
    {
        if (k == DOT || k == ' ')
        {
            if (l == DOT || l == ' ')
            {
                if (j == DOT || j == ' ') return ACS_HLINE;
                else          return ACS_URCORNER;
            }
            else
            {
                if (j == DOT || j == ' ') return ACS_HLINE;
                else          return ACS_TTEE;
            }
        }
        else
        {
            if (l == DOT || l == ' ')
            {
                if (j == DOT || j == ' ') return ACS_LRCORNER;
                else          return ACS_RTEE;
            }
            else
            {
                if (j == DOT || j == ' ') return ACS_BTEE;
                else
                {
                    if (y == DOT || u == DOT || b == DOT || n == DOT)
                              return ACS_PLUS;
                    else      return ' ';
                }
            }
        }
    }
}

#define US(w) (sq_seen[w]?(sq_attr[w]?DOT:'W'):DOT)

inline void set_can_see(int Yloc, int Xloc, uint32_t *us)
{
    int Y,X,w;
    int I;
    bres_start (Yloc, Xloc, sq_seen, sq_attr);
    for (w = 0; w < 1680; ++ w)
        if(sq_seen[w] == 2) sq_seen[w] = 1;
    for (Y = 0; Y < 21; ++ Y)
        for (X = 0; X < 80; ++ X)
            bres_draw(Y,X);
    for (w = 0; w < 1680; ++ w)
    {
        if (!sq_seen[w]) us[w] = ' ';
    }
    for (Y = 0; Y < 21; ++ Y)
    {
        for (X = 0; X < 80; ++ X)
        {
            w = to_buffer(Y,X);
            if (sq_attr[w] == 2) us[w] |= COL_TXT_BRIGHT;
            if (sq_attr[w] != 0 || us[w] == ' ') continue;
            uint32_t y='.', u='.', h='.', j='.', k='.', l='.', b='.', n='.';
            if(X) h = US(w-1);
            if(Y) k = US(w-80);
            if(X<79) l = US(w+1);
            if(Y<20) j = US(w+80);
            if(X && Y) y = US(w-81);
            if(X<79 && Y) u = US(w-79);
            if(X && Y<20) b = US(w+79);
            if(X<79 && Y<20) n = US(w+81);
            if (us[w] == COL_TXT_BRIGHT) us[w] |= WALL_TYPE(y,u,h,j,k,l,b,n);
            else us[w] = WALL_TYPE(y,u,h,j,k,l,b,n);
        }
    }
}

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
 * 160 161 ...                        21
 * 240 ...                            |
 * ...                       ...      -
 *  					... 1679
 *  |-------------80-------------|     */
/* The bottom three lines of the 80*25 console are used for HP, stats, time, etc
 * the top is used for pline(). A null character signifies nothing - the character
 * already there should not be overwritten. */

int* visualise_map ()
{
	int I;
	struct list_iter *i;
    struct Thing *player = NULL;
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
        /* assert(at < 1680); */
        switch(th.type)
        {
            case THING_MONS:
            {
				struct Monster *m = th.thing;//pline("%d", m);getch();
                changed = true;
				map[at] = mons[m->type].col | mons[m->type].ch;
				if (m->name)
                    if (m->name[0] == '_')
                    {
					    map[at] |= COL_TXT_BRIGHT;
                        player = T;
                    }
                sq_attr[at] = 1;
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
                sq_attr[at] = 1;
                break;
            }
            case THING_DGN:
            {
				if (type[at] == THING_NONE)
				{
					struct map_item_struct *m = th.thing;
                    map[at] = (uint32_t)((unsigned char)(m->ch));
                    sq_attr[at] = m->attr&1;
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
    set_can_see(player->yloc, player->xloc, map);
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
    struct Thing *t;
	for(i = all_things.beg; iter_good(i); next_iter(&i))
    {
        t = i->data;
        if (t->type == THING_MONS)
        {
            struct Monster* mon = (struct Monster*)(t->thing);
            if(mon->name[0] == '_') /* player */
            {
                return(t);
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
