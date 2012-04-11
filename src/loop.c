/* loop.c
 * Linden Ralph */

#include "include/all.h"
#include "include/loop.h"
#include "include/pline.h"
#include "include/monst.h"
#include "include/rand.h"
#include "include/thing.h"
#include "include/output.h"
#include "include/generate.h"

uint64_t Time = 0;

void next_time()
{
    ++ Time;
    if (digesting()) ++ U.hunger;
}

void main_loop()
{
    char  *msg = 0;
    struct Thing *pl = get_player(), *th;
    struct Monster *pl_mon = pl->thing, *mon;
    struct list_iter* i;
    next_time();
    mons_gen(2, -55);
    for(i = all_things.beg; iter_good(i); next_iter(&i))
    {
        th = i->data;
        if (th->type != THING_MONS) continue;

        mon = th->thing;
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
        if (pl_mon->HP <= 0)
        {
            U.playing = PLAYER_LOSTGAME;
            return;
        }

        /* The player can live with no dexterity and/or charisma, but there are
         * other penalties (fumbling, aggravation etc). */
        if (U.attr[AB_ST] <= 0) msg = "weakness";
        if (U.attr[AB_CO] <= 0) msg = "flabbiness";
        if (U.attr[AB_IN] <= 0) msg = "brainlessness";
        if (U.attr[AB_WI] <= 0) msg = "foolishness";
        if (pl_mon->level <= 0)       msg = "inexperience";
        if (U.hunger > HN_LIMIT_5)    msg = "hunger";
        if (U.hunger <= 1)            msg = "consumption";

        if (msg)
        {
            player_dead("You die of %s.", msg);
            U.playing = PLAYER_LOSTGAME;
            return;
        }
    }
}
