/* rank.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/rank.h"
#include "include/monst.h"
#include "include/pline.h"
#include <assert.h>

struct Rankings
{
	char *ranks[6];
};

struct Rankings all_ranks[] =
	{ {{0, 0, 0, 0, 0, 0}},
	{{"Private", "Corporal", "Liutenant", "Captain", "Seargant", "General"}},
	{{"Dentist", "Nurse", "Assistant", "GP", "Specialist", "Surgeon"}},
	{{"Thug", "Butcher", "Hitman", "Mercenary", "Cutthroat", "Executioner"}} };

char *get_rank ()
{
	uint32_t level = pmons.level;

	if (level == 0)
		return NULL;

	assert(level <= 30);

	return all_ranks[U.role].ranks[(level - 1) / 5];
}

int level_boundary[30] =
	{ 0, 10, 20, 30, 50, 80, 130, 210, 340, 550, 890, 1440 };

void update_level (struct Thing *th)
{
	uint32_t exp = th->thing.mons.exp;
	struct Monster *mon = &th->thing.mons;
	int i;

	for (i = 0; level_boundary[i] <= exp; ++i)
		exp -= level_boundary[i];

	if (th == player)
	{
		if (i > mon->level)
		{
			pline("Level up! You are now level %d.", i);
			mon->HP_max += mon->level * 3 + 2;
		}
		if (i < mon->level)
			pline("Level down... You dropped to level %d.", i);
	}
	mon->level = i;
}
