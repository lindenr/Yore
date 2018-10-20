/* skills.c */

#include "include/skills.h"
#include "include/drawing.h"
#include "include/panel.h"
#include "include/event.h"

const styp all_skills[] = {
	{SK_NONE, 0, 0, "", ""},
	{SK_CHARGE, SK_ACT, GL_STR, "Charge",  "#CHARGE\n#[cost: 3 stamina/square]\n\nCharge at your opponent, dealing extra damage for one hit. Be careful not to miss! Damage increases with skill level."},
	{SK_DODGE, SK_PAS, GL_AGI, "Dodge", "#DODGE\n#[passive skill]\n\nYou are uncommonly nimble. When you are attacked you have a chance to dodge the attack. The chance increases with skill level."},
	{SK_FIREBALL, SK_ACT, GL_WIS, "Fireball", "#FIREBALL\n#[cost: 5 MP]\nfireball!"},
	{SK_WATER_BOLT, SK_ACT, GL_WIS, "Water bolt", "#WATER BOLT\n#[cost: 6 MP]\nWater!"},
	{SK_FROST, SK_ACT, GL_WIS, "Frost", "#FROST\n[cost: free]\nLightly freezes an area."},
	{SK_SCRY,  SK_ACT, GL_WIS, "Scry", "#SCRY\n#[cost: free]\nscry!!!!"},
	{SK_FLASH,  SK_ACT, GL_AGI, "Flash", "#FLASH\n#[cost: free]\nYou briefly become inhumanly fast."},
	{SK_USE_MARTIAL_ARTS, SK_PAS, GL_STR, "Martial arts ability", "#MARTIAL ARTS\n\nYour skill at using martial arts in combat."},
	{SK_USE_LONGSWORD, SK_PAS, GL_STR, "Longsword ability", "#LONGSWORD\n\nYour skill at using a longsword in combat."},
	{SK_USE_AXE, SK_PAS, GL_STR, "Axe ability", "#AXE\n\nYour skill at using an axe in combat."},
	{SK_USE_HAMMER, SK_PAS, GL_STR, "Hammer ability", "#HAMMER\n\nYour skill at using a hammer in combat."},
	{SK_USE_DAGGER, SK_PAS, GL_STR, "Dagger ability", "#DAGGER\n\nYour skill at using dagger in combat."},
	{SK_USE_SHORTSWORD, SK_PAS, GL_STR, "Short sword ability", "#SHORT SWORD\n\nYour skill at using short sword in combat."}
};

#define SK_MAXLEVEL ((int)(sizeof(xp_levels)/sizeof(*xp_levels)))
const int xp_levels[] = {0, 5};

const char *sk_name (Skill sk)
{
	return all_skills[sk->type].name;
}

const char *sk_desc (Skill sk)
{
	return all_skills[sk->type].desc;
}

glyph sk_gl (Skill sk)
{
	return all_skills[sk->type].gl;
}

int sk_isact (Skill sk)
{
	return all_skills[sk->type].flags == SK_ACT;
}

int sk_lvl (struct Monster *th, enum SK_TYPE type)
{
	int i;
	Vector sk_vec = th->skills;
	for (i = 0; i < sk_vec->len; ++ i)
	{
		Skill sk = v_at (sk_vec, i);
		if (sk->type != type)
			continue;
		return sk->level;
	}
	return 0;
}

void sk_fireball (struct Monster *mons, int yloc, int xloc, Skill sk)
{
	if (mons->MP < 5)
		return;
	mons->MP -= 5;
	ev_queue (mons->speed/2, (union Event) { .mfireball = {EV_MFIREBALL, mons->ID, yloc, xloc, sk->level*5}});
	mons_ex_skill (mons, sk);
}

void sk_water_bolt (struct Monster *mons, int yloc, int xloc, Skill sk)
{
	if (mons->MP < 6)
		return;
	mons->MP -= 6;
	ev_queue (mons->speed/2, (union Event) { .mwater_bolt = {EV_MWATER_BOLT, mons->ID, yloc, xloc, sk->level*5}});
	mons_ex_skill (mons, sk);
}

void sk_frost (struct Monster *mons, int yloc, int xloc, Skill sk)
{
	ev_queue (0, (union Event) { .mfrost = {EV_MFROST, mons->ID, yloc, xloc, 3}});
	mons_ex_skill (mons, sk);
}

void sk_charge (struct Monster *th, int y, int x, Skill sk)
{
	if (!th->status.charging)
	{
		ev_queue (0, (union Event) { .mstartcharge = {EV_MSTARTCHARGE, th->ID /*, y, x*/ }});
		return;
	}
	ev_queue (0, (union Event) { .mstopcharge = {EV_MSTOPCHARGE, th->ID /*, y, x*/ }});
}

void sk_scry (struct Monster *mons, Skill skill)
{
}

void sk_flash (struct Monster *mons, Skill skill)
{
	ev_queue (0, (union Event) { .mflash = {EV_MFLASH, mons->ID, 107, 500}});
}

