/* generate.c
 * Linden Ralph */

#include "all.h"
#include "generate.h"
#include "rand.h"
#include "thing.h"
#include "monst.h"
#include "map.h"

void generate_map(enum LEVEL_TYPE type)
{
    int i, x, Y, start;
    int buffer[1680];
    for (i = 0; i < 1680; ++ i)
    {
        buffer[i] = ' ';
    }
    /* much better 8) */
    if (type == LEVEL_MINES)
    {
        int t = 200;
        start = RN(1520)+79;start=85;
        buffer[start] = ' ';
        buffer[mons_gen(0, RN(1520))] = ' ';
        while (t--) buffer[RN(1520)+79]=DOT;

        t = 800;
        while (t--)
        {
            int buf = RN(1520)+79;
            if ((buffer[buf] != DOT) && ((buffer[buf+1]==DOT)||(buffer[buf-1]==DOT)
                                         ||(buffer[buf+80]==DOT)||(buffer[buf-80]==DOT)))
                buffer[buf]=DOT;
            else
                t++;
        }
        for (i = 80; i < 1680; ++ i)
            if(buffer[i] != DOT) buffer[i] = 'W';
        for(i = 0; i < 1680; ++ i)
        {
            if (buffer[i] == ' ') continue;
            struct map_item_struct *mis = malloc(sizeof(struct map_item_struct));
            memcpy(mis, &(map_items[GETMAPITEMID(buffer[i])]), sizeof(struct map_item_struct));
            new_thing(THING_DGN, i/80, i%80, mis);
        }
    }
    else if (type == LEVEL_NORMAL)
    {

    }
    else if (type == LEVEL_FOREST)
    {/*
        struct List lis = get_level_as_list("level_forest");
        struct list_iter *li;

        for (li = lis.beg; iter_good(li); next_iter(&li))
        {
            
        }*/ /* Do all that later */
    }
}

/* can a monster be generated here? */
bool is_safe(uint32_t yloc, uint32_t xloc)
{
    struct list_iter *i;
    struct Thing *T;
    struct map_item_struct *m;
    for (i = all_things.beg; iter_good(i); next_iter(&i))
    {
        T = i->data;
        if (T->type == THING_MONS && T->yloc == yloc && T->xloc == xloc) return false;
        if (T->type == THING_DGN  && T->yloc == yloc && T->xloc == xloc)
        {
            m = T->thing;
            if (!m->attr&1) return false;
        }
    }
    return true;
}

/* type:
 * 0 = initialised at start of game,
 * 1 = generated at start of level,
 * 2 = randomly throughout level    */
char nm[30] = {'_', 'L', 0,};
struct Monster Pl[] = {{1, 0, 20, 20, 0, nm, {0,}, {0,}, {5,5,5,5,5,5}}};

uint32_t mons_gen(int type, int32_t param)
{
    int32_t luck, start;
    uint32_t end;
    struct map_item_struct *mis, *mise;
    if (type == 0)
    {
        start = param;
        new_thing(THING_MONS, start/80, start%80, Pl);
        mis = malloc(sizeof(struct map_item_struct));
        memcpy(mis, &(map_items[GETMAPITEMID('<')]), sizeof(struct map_item_struct));
        new_thing(THING_DGN, start/80, start%80, mis);
        do end = RN(1520);
        while(end == start);
        mise = malloc(sizeof(struct map_item_struct));
        memcpy(mise, &(map_items[GETMAPITEMID('>')]), sizeof(struct map_item_struct));
        new_thing(THING_DGN, end/80, end%80, mise);
        return end;
    }
    else if (type == 2)
    {
        luck = param;
        if (RN(500) < (10-luck))
        {
            struct Monster *p = malloc(sizeof(struct Monster));
            memset(p, 0, sizeof(struct Monster));
            p->type = RN(5)-1;
            p->HP = 2;
            p->name = "";
            p->attr[AB_ST] = 5;
            uint32_t xloc = RN(75), yloc = RN(15);
            if (is_safe(yloc, xloc))
                new_thing(THING_MONS, yloc, xloc, p);
        }
    }
}
