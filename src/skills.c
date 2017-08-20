/* skills.c */

#include "include/skills.h"
#include "include/vision.h"
#include "include/panel.h"

const styp all_skills[] = {
	{SK_NONE, 0, "", ""},
	{SK_CHARGE,  SK_ACT, "Charge",  "#CHARGE\n#[cost: 3 stamina/square]\n\nCharge at your opponent, dealing extra damage for one hit. Be careful not to miss! Damage increases with skill level.\n"},
	{SK_DODGE,   SK_PAS, "Dodge", "#DODGE\n#[passive skill]\n\nYou are uncommonly nimble. When you are attacked you have a chance to dodge the attack. The chance increases with skill level.\n"}
};

#define SK_MAXLEVEL (sizeof(xp_levels)/sizeof(*xp_levels))
const int xp_levels[] = {0, 5};

const char *sk_name (Skill sk)
{
	return all_skills[sk->type].name;
}

const char *sk_desc (Skill sk)
{
	return all_skills[sk->type].desc;
}

int sk_isact (Skill sk)
{
	return all_skills[sk->type].flags == SK_ACT;
}

int sk_lvl (struct Thing *th, enum SK_TYPE type)
{
	int i;
	Vector sk_vec = th->thing.mons.skills;
	for (i = 0; i < sk_vec->len; ++ i)
	{
		Skill sk = v_at (sk_vec, i);
		if (sk->type != type)
			continue;
		return sk->level;
	}
	return 0;
}

void sk_exp (struct Thing *th, Skill sk, int xp)
{
	sk->exp += xp;
	if (sk->level == SK_MAXLEVEL || sk->exp < xp_levels[sk->level])
		return;
	++ sk->level;
	if (sk->level < SK_MAXLEVEL && sk->exp >= xp_levels[sk->level])
		sk->exp = xp_levels[sk->level]-1;
	if (mons_isplayer(th))
		p_msg ("Level up! %s is now level %d", sk_name (sk), sk->level);
}

#define SK_CHARGE_COST 3
int chID = 0;
int chargepos (struct DLevel *dlevel, int y, int x)
{
	struct Thing *charger = THIID (chID);
	int dy = y - charger->yloc, dx = x - charger->xloc;
	if (dx*dx > 1 || dy*dy > 1)
	{
		fprintf(stderr, "AAAAAAA\n");
		return 0;
	}
	if (charger->thing.mons.ST < SK_CHARGE_COST)
	{
		p_msg ("You run out of breath.");
		return 0;
	}
	charger->thing.mons.ST -= SK_CHARGE_COST;
	return mons_move (charger, dy, dx, 0)==1;
}

void sk_charge (struct Thing *th, int y, int x, Skill sk)
{
	sk_exp (th, sk, 1);
	
	chID = th->ID;
	mons_usedturn (th);
	th->thing.mons.status |= M_CHARGE;
	bres_draw (th->yloc, th->xloc, NULL, dlv_attr(th->dlevel), &chargepos, y, x);
}

