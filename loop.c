/* loop.c
 * Linden Ralph */

#include "all.h"
#include "loop.h"
#include "pline.h"
#include "monst.h"
#include "rand.h"
#include "thing.h"
#include "output.h"

uint64_t Time = 0;

bool main_loop()
{
    char *msg = 0;
    char in;
    struct Thing *pl = get_player();
    struct Monster *mn = pl->thing;
    struct list_iter* i;
    ++Time;
    mvprintw(23, 0, "Time: %d", Time);
    mons_gen(-15); /* certain to generate monsters - TODO change me! */
    for(i = all_things.beg; iter_good(i); next_iter(&i))
    {
        if (((struct Thing*)(i->data))->type == THING_MONS)
        {
            struct Monster* mon = (struct Monster*)((struct Thing*)(i->data))->thing;
            mon->cur_speed += mons[mon->type].speed;
            while (mon->cur_speed >= 12)
            {
                move(pl->yloc+1, pl->xloc);
                mon->cur_speed -= 12;
                if(!mons_take_move(mon)) return false;
                update_map();
            }
            move(pl->yloc+1, pl->xloc);
            if (mn->HP <= 0) return false;
            if (mn->attr[AB_ST] <= 0) msg = "weakness";
            if (mn->attr[AB_CO] <= 0) msg = "flabbiness";
            if (mn->attr[AB_IN] <= 0) msg = "brainlessness";
            if (mn->attr[AB_WI] <= 0) msg = "foolishness";
            /* The player can live with no dexterity and/or charisma, but there are
             * other penalties (fumbling, aggravation etc). */
            if (msg)
            {
                player_dead("You die of %s.", msg);
                return false;
            }
        }
    }
    return true;
}
