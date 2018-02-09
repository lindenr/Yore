/* skills.c */

#include "include/skills.h"
#include "include/drawing.h"
#include "include/panel.h"
#include "include/event.h"

const styp all_skills[] = {
	{SK_NONE, 0, "", ""},
	{SK_CHARGE,  SK_ACT, "Charge",  "#CHARGE\n#[cost: 3 stamina/square]\n\nCharge at your opponent, dealing extra damage for one hit. Be careful not to miss! Damage increases with skill level.\n"},
	{SK_DODGE,   SK_PAS, "Dodge", "#DODGE\n#[passive skill]\n\nYou are uncommonly nimble. When you are attacked you have a chance to dodge the attack. The chance increases with skill level.\n"},
	{SK_FIREBALL, SK_ACT, "Fireball", "#FIREBALL\n#[cost: 5 MP]\nfireball!\n"},
	{SK_WATER_BOLT, SK_ACT, "Water bolt", "#WATER BOLT\n#[cost: 6 MP]\nWater!\n"},
	{SK_FROST, SK_ACT, "Frost", "#FROST\n[cost: free]\nLightly freezes an area."},
	{SK_FLAMES,  SK_ACT, "Circle of flame", "#CIRCLE OF FLAME\n#[cost: free]\nfiire!!!!\n"}
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
	ev_queue (mons->speed, (union Event) { .mfireball = {EV_MFIREBALL, mons->ID, yloc, xloc}});
	ev_queue (mons->speed+1, (union Event) { .mturn = {EV_MTURN, mons->ID}});
}

void sk_water_bolt (struct Monster *mons, int yloc, int xloc, Skill sk)
{
	if (mons->MP < 6)
		return;
	mons->MP -= 6;
	ev_queue (mons->speed, (union Event) { .mwater_bolt = {EV_MWATER_BOLT, mons->ID, yloc, xloc}});
	ev_queue (mons->speed+1, (union Event) { .mturn = {EV_MTURN, mons->ID}});
}

void sk_frost (struct Monster *mons, int yloc, int xloc, Skill sk)
{
	ev_queue (0, (union Event) { .mfrost = {EV_MFROST, mons->ID, yloc, xloc, 3}});
	ev_queue (mons->speed+1, (union Event) { .mturn = {EV_MTURN, mons->ID}});
}

#if 0
void sk_exp (struct Monster *th, Skill sk, int xp)
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
	struct Monster *charger = MTHIID (chID);
	int dy = y - charger->yloc, dx = x - charger->xloc;
	if (dx*dx > 1 || dy*dy > 1)
	{
		fprintf(stderr, "AAAAAAA\n");
		return 0;
	}
	if (charger->ST < SK_CHARGE_COST)
	{
		p_msg ("You run out of breath.");
		return 0;
	}
	charger->ST -= SK_CHARGE_COST;
	return mons_move (charger, dy, dx)==1;
}
#endif
void sk_charge (struct Monster *th, int y, int x, Skill sk)
{
	if (!th->status.charging)
	{
		ev_queue (0, (union Event) { .mturn = {EV_MTURN, th->ID}});
		ev_queue (0, (union Event) { .mstartcharge = {EV_MSTARTCHARGE, th->ID /*, y, x*/ }});
		return;
	}
	ev_queue (0, (union Event) { .mturn = {EV_MTURN, th->ID}});
	ev_queue (0, (union Event) { .mstopcharge = {EV_MSTOPCHARGE, th->ID /*, y, x*/ }});
//	sk_exp (th, sk, 1);
	
//	chID = th->ID;
//	th.status |= M_CHARGE;
//	bres_draw (th->yloc, th->xloc, NULL, dlv_attr(th->dlevel), &chargepos, y, x);
}

void sk_flames (struct Monster *th)
{
	ev_queue (0, (union Event) { .circleofflame = {EV_CIRCLEOFFLAME, th->ID}});
}

Graph flames_overlay = NULL;
void sk_flames_overlay (struct Monster *th)
{
	flames_overlay = gra_init (gr_h, gr_w, 0, 0, gr_h, gr_w);
	gra_clear (flames_overlay);
	int y = th->yloc, x = th->xloc;
	int i, yt, xt, max = 20;
	for (i = 1; i < max; ++ i)
	{
		int i2 = (i-1)*(i-1), j2 = (i+1)*(i+1);
		for (yt = -max; yt <= max; ++ yt) for (xt = -max; xt <= max; ++ xt)
		{
			int r2 = xt*xt + yt*yt;
			if (r2 < i2 || r2 > j2)
			{
				gra_mvaddch (flames_overlay, y + yt - map_graph->cy, x + xt - map_graph->cx, 0);
				continue;
			}
			int redness = ((15-((j2+i2-r2-r2)*(j2+i2-r2-r2)*15)/((j2-i2)*(j2-i2)))*(max-i))/max;
			if (redness <= 5)
			{
				gra_mvaddch (flames_overlay, y + yt - map_graph->cy, x + xt - map_graph->cx, 0);
				continue;
			}
			int out[] = {30, 30, 30, 30};//{'/', ACS_HLINE, '\\', ACS_VLINE};
			glyph gl = out[yt?(xt?2*((yt>0)!=(xt>0)):1):3]|COL_BG_RED(redness)|COL_TXT(15,redness/2,0);
			gra_mvaddch (flames_overlay, y + yt - map_graph->cy, x + xt - map_graph->cx, gl);
		}
		gr_refresh ();
		gr_wait (50);
	}
	gra_free (flames_overlay);
	ev_queue (0, (union Event) { .mturn = {EV_MTURN, th->ID}});
}

