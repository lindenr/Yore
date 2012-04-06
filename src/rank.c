/* rank.c
 * Linden Ralph */

#include "include/all.h"
#include "include/rank.h"
#include "include/monst.h"

struct Rankings
{
    char *ranks[6];
};

struct Rankings all_ranks[] = {{0, 0, 0, 0, 0, 0}, {"Private", "Corporal", "Liutenant", "Captain", "Seargant", "Major"}, {"Dentist", "Nurse", "Assistant", "GP", "Specialist", "Surgeon"}};

char *get_rank()
{
    int i;
    struct Monster *mon = U.player->thing;
    uint32_t level = mon->level;
    
    if (level == 0) return NULL;

    for (i = 0; i < 6; ++ i)
    {
        if (level <= 5) return all_ranks[U.role].ranks[i];
        level -= 5;
    }

    return NULL;
}

int level_boundary[30] = {0, 30, 50, 100, 160, 300, 500, 800, 1200,};

void update_level(struct Monster *mon)
{
    uint32_t exp = mon->exp;
    int i;
    for (i = 0; level_boundary[i] <= exp; ++ i) exp -= level_boundary[i];
    
    if (mon->name[0] == '_')
    {
        if (i > mon->level) pline("Level up! You are now level %d.", i);
        if (i < mon->level) pline("Level down... You dropped to level %d.", i);
    }
    mon->level = i;
}

