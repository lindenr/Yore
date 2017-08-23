/* action.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/monst.h"
#include "include/rand.h"

//void mons_regen (struct Thing *th) // ACTION
//{
	/*struct Monster *self = &th->thing.mons;

	/ * HP * /
	if (rn(50) < U.attr[AB_CO])
		self->HP += (self->level + 10) / 10;
	if (self->HP > self->HP_max)
		self->HP = self->HP_max;
	self->HP_rec = ((10.0 + self->level)/10) * ((float)U.attr[AB_CO] / 50.0);

	/ * ST * /
	self->ST += rn(2);
	if (self->ST > self->ST_max)
		self->ST = self->ST_max;
	self->ST_rec = 0.5;*/
//}
/*
void act_mhit (struct Thing *from, struct Thing *to)
{
	struct Item **it = mons_get_weap(from);
	int dmg, strength;
	if (!it || !(*it))
	{
		strength = rn(mons_get_st(from)) >> 1;
		dmg =
//			rnd(mons[type].attacks[t][0],
//				mons[type].attacks[t][1]) +
			strength +
			rn(1 + 3*from->thing.mons.level);
	}
	else
	{
		int attr = (*it)->type.attr;
		strength = rn(mons_get_st(from)) >> 1;
		dmg = rnd(attr & 15, (attr >> 4) & 15) + strength;
		//printf("%d %d\n", *toHP, *toHP - damage);
	}

	to->thing.mons.HP -= dmg;// + bonus;
//	mons_passive_attack (to, from);
}

void act_mtouch (struct Thing *to, struct Thing *from)
{
	to->thing.mons.HP -= 1;
//		rnd(mons[type].attacks[t][0],
//			mons[type].attacks[t][1]);

//	mons_passive_attack (to, from);
}

void act_mclaw (struct Thing *to, struct Thing *from)
{
	to->thing.mons.HP -= 1;
//		rnd(mons[type].attacks[t][0],
//			mons[type].attacks[t][1]);

//	mons_passive_attack (to, from);
}

void act_mbite (struct Thing *to, struct Thing *from)
{
	to->thing.mons.HP -= 1;
//		rnd(mons[type].attacks[t][0],
//			mons[type].attacks[t][1]);

//	mons_passive_attack (to, from);
}*/

