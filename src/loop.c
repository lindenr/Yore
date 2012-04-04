/* loop.c
 * Linden Ralph */

#include "include/all.h"
#include "include/loop.h"
#include "include/pline.h"
#include "include/monst.h"
#include "include/rand.h"
#include "include/thing.h"
#include "include/output.h"

uint64_t Time = 0;

void next_time()
{
    ++ Time;
    if (digesting()) ++ U.hunger;
}

void main_loop()
{
    char  *msg = 0;
    struct Thing *pl = get_player();
    struct Monster *mn = pl->thing;
    struct list_iter* i;
    next_time();
    mons_gen(2, -15);
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
                /* U.player == PLAYER_LOSTGAME if this happens */
                if(!mons_take_move(mon)) return;
                update_map();
            }
            move(pl->yloc+1, pl->xloc);
            if (mn->HP <= 0)
            {
                U.playing = PLAYER_LOSTGAME;
                return;
            }
            /* The player can live with no dexterity and/or charisma, but there are
             * other penalties (fumbling, aggravation etc). */
            if (mn->attr[AB_ST] <= 0) msg = "weakness";
            if (mn->attr[AB_CO] <= 0) msg = "flabbiness";
            if (mn->attr[AB_IN] <= 0) msg = "brainlessness";
            if (mn->attr[AB_WI] <= 0) msg = "foolishness";
			if (U.hunger > ABSOLUTE_HUNGER_LIMIT) msg = "hunger";
			if (U.hunger <= 1) msg = "consumption";
            if (msg)
            {
                player_dead("You die of %s.", msg);
                U.playing = PLAYER_LOSTGAME;
                return;
            }
        }
    }
}
