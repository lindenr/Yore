/* generate.c
 * Linden Ralph */

#include "include/all.h"
#include "include/generate.h"
#include "include/rand.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/map.h"

uint32_t upsy, upsx, downsy, downsx;

void get_upstair(uint32_t *yloc, uint32_t *xloc)
{
    *yloc = upsy;
    *xloc = upsx;
}

void get_downstair(uint32_t *yloc, uint32_t *xloc)
{
    *yloc = downsy;
    *xloc = downsx;
}

void generate_map(enum LEVEL_TYPE type)
{
    int i, x, Y, start;
    int buffer[1680];
    for (i = 0; i < 1680; ++ i)
    {
        buffer[i] = ' ';
    }
    if (type == LEVEL_MINES)
    {
        int t = 200;
        start = RN(1520) + 79;
        mons_gen(0, start);

        /* clear space at the beginning (for the up-stair) */
        buffer[start] = DOT;

        /* clear space for the down-stair */
        buffer[mons_gen(1, start)] = DOT;

        /* generate some random spaces */
        while (t--) buffer[RN(1520)+79] = DOT;

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

        /* Will be a wall if it is at edge of screen, or if it is not a space. */
        for (i = 0; i < 1680; ++ i)
            if (buffer[i] != DOT || ((i+1)%80) <= 1) buffer[i] = 'W'; 

        /* Add everything to the list. */
        for(i = 0; i < 1680; ++ i)
        {
            struct map_item_struct *mis = malloc(sizeof(struct map_item_struct));
            memcpy(mis, &(map_items[GETMAPITEMID(buffer[i])]), sizeof(struct map_item_struct));
            new_thing(THING_DGN, i/80, i%80, mis);
        }
    }
    else if (type == LEVEL_NORMAL)
    {
        /* TODO */
    }
    else if (type == LEVEL_FOREST)
    {/*
        struct List lis = get_level_as_list("level_forest");
        struct list_iter *li;

        for (li = lis.beg; iter_good(li); next_iter(&li))
        {
            
        }*/ /* TODO */
    }
}

/* can a monster be generated here? (no monsters or walls in the way) */
bool is_safe_gen(uint32_t yloc, uint32_t xloc)
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

char real_player_name[20] = "_";
struct Monster Pl[] = {{1, 1, 20, 20, 0, real_player_name, {0,}, {0,}, {5,5,5,5,5,5}, 0, 0}};

/* type:
 * 0 = initialised at start of game,
 * 1 = generated at start of level,
 * 2 = randomly throughout level    */
uint32_t mons_gen(int type, int32_t param)
{
    int32_t luck, start;
    uint32_t end;
    struct map_item_struct *mis, *mise;
    if (type == 0)
    {
        start = param;
        upsy = start/80; upsx = start%80;
        U.player = new_thing(THING_MONS, upsy, upsx, Pl);
    }
    else if (type == 1)
    {
        start = param;
        upsy = start/80; upsx = start%80;
        mis = malloc(sizeof(struct map_item_struct));
        memcpy(mis, &(map_items[GETMAPITEMID('<')]), sizeof(struct map_item_struct));
        new_thing(THING_DGN, upsy, upsx, mis);
        do end = RN(1520) + 79;
        while(end == start);
        downsy = end/80; downsx = end%80;
        mise = malloc(sizeof(struct map_item_struct));
        memcpy(mise, &(map_items[GETMAPITEMID('>')]), sizeof(struct map_item_struct));
        new_thing(THING_DGN, downsy, downsx, mise);
        return end;
    }
    else if (type == 2)
    {
        luck = param;
        if (RN(500) < (10-luck))
        {
            struct Monster *p = malloc(sizeof(struct Monster));
            memset(p, 0, sizeof(struct Monster));
            p->type = RN(6)-1;
            p->HP = 20;
            p->HP_max = 20;
            p->name = "";
            p->attr[AB_ST] = 5;
            uint32_t xloc = RN(75), yloc = RN(15);
            if (is_safe_gen(yloc, xloc))
                new_thing(THING_MONS, yloc, xloc, p);
        }
    }
}
