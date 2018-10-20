/* monst.c */

#include "include/thing.h"
#include "include/panel.h"
#include "include/rand.h"
#include "include/drawing.h"
#include "include/dlevel.h"
#include "include/monst.h"
#include "include/player.h"
#include "include/event.h"
#include "include/skills.h"
#include "include/debug.h"

#include <stdio.h>

struct player_status U;

int expcmp (int p_exp, int m_exp)
{
	if (p_exp < 21)
		return (m_exp < 7);
	return p_exp >= ((m_exp*(m_exp-1))/2);
}

int nogen (int mons_id)
{
	if (mons_id == MTYP_Satan)
		return ((U.m_glflags&MGL_GSAT) != 0);

	return 0;
}

const int explevel[] = {0, 20, 40, 80, 160, 320, 640, 1250, 2500, 5000, 10000};
const int maxlevel = sizeof(explevel)/sizeof(explevel[0]);

int mons_gen_type ()
{
	//return MTYP_skeleton;
	int i, array[MTYP_NUM_MONS];
	int level = MTHIID(*(TID*)v_at(cur_dlevel->playerIDs,0))->level;
	uint32_t p_exp = level < maxlevel ? explevel[level] : explevel[maxlevel-1]*2;
	uint32_t total_weight = 0;

	for (i = 0; i < MTYP_NUM_MONS; ++i)
	{
		if (nogen(i)) continue; /* genocided or unique and generated etc */
		array[i] = expcmp(p_exp, all_mons[i].exp);
		total_weight += array[i];
	}

	int r = rn(total_weight);
	for (i = 0; i < MTYP_NUM_MONS; ++ i)
	{
		if (array[i] > r)
			return i;
		r -= array[i];
	}

	/* If the player has no exp this will happen */
	return 0;
}

int mons_gets_exp (struct Monster *mons)
{
	if (!mons)
		return 0;
	return mons_isplayer (mons);
}

int mons_level (int exp)
{
	int i, ret = 0;
	for (i = 0; i < maxlevel; ++ i)
	{
		if (exp >= explevel[i])
			++ ret;
		else
			break;
	}
	return ret;
}

int mons_exp_needed (int level)
{
	if (level < maxlevel)
		return explevel[level];
	return -1;
}

int is (struct Monster *mons)
{
	return mons && mons->ID;
}

void mons_corpse (struct Monster *mons, struct Item *item)
{
	*item = new_item (ITYP_NUM_ITEMS + mons_type (mons));
}

enum MTYPE mons_type (const struct Monster *mons)
{
	if (!mons)
		panic ("NULL mons in mons_type");
	return mons->mtype;
}

int mons_get_HP (struct Monster *mons)
{
	return 2*mons->str + 1;
}

int mons_get_ST (struct Monster *mons)
{
	if (mons->con <= 5)
		return (3*mons->con)/2 + 5;
	return (4*mons->con) - 8;
}

int mons_get_MP (struct Monster *mons)
{
	return mons->wis + 1;
}

int mons_index (const struct Monster *mons)
{
	struct DLevel *lvl = dlv_lvl (mons->dlevel);
	return dlv_index (lvl, mons->zloc, mons->yloc, mons->xloc);
}

int mons_can_wear (struct Monster *mons, struct Item *it, size_t offset)
{
	if (it_canwear (it, MONS_HAND))
		return (offset >= offsetof (struct WoW, hands[0]) &&
			offset < offsetof (struct WoW, hands[mons->wearing.narms]));
	if (it_canwear (it, MONS_TORSO))
		return (offset >= offsetof (struct WoW, torsos[0]) &&
			offset < offsetof (struct WoW, torsos[mons->wearing.ntorsos]));
	if (it_canwear (it, MONS_HEAD))
		return (offset >= offsetof (struct WoW, heads[0]) &&
			offset < offsetof (struct WoW, heads[mons->wearing.nheads]));
	return 0;
}

int mons_try_wear (struct Monster *mons, struct Item *it)
{
	int i;
	if (it_canwear (it, MONS_HAND))
	{
		for (i = 0; i < mons->wearing.narms; ++ i)
		{
			if (!mons->wearing.hands[i])
				break;
		}
		if (i >= mons->wearing.narms)
		{
			p_msg ("You don't have enough hands!");
			return 0;
		}
		ev_queue (0, (union Event) { .mwear_armour =
			{EV_MWEAR_ARMOUR, mons->ID, it->ID, offsetof (struct WoW, hands[i])}});
		return 1;
	}
	if (it_canwear (it, MONS_TORSO))
	{
		for (i = 0; i < mons->wearing.ntorsos; ++ i)
		{
			if (!mons->wearing.torsos[i])
				break;
		}
		if (i >= mons->wearing.ntorsos)
		{
			p_msg ("You don't have enough torsos!");
			return 0;
		}
		ev_queue (0, (union Event) { .mwear_armour =
			{EV_MWEAR_ARMOUR, mons->ID, it->ID, offsetof (struct WoW, torsos[i])}});
		return 1;
	}
	if (it_canwear (it, MONS_HEAD))
	{
		for (i = 0; i < mons->wearing.nheads; ++ i)
		{
			if (!mons->wearing.heads[i])
				break;
		}
		if (i >= mons->wearing.nheads)
		{
			p_msg ("You don't have enough heads!");
			return 0;
		}
		ev_queue (0, (union Event) { .mwear_armour =
			{EV_MWEAR_ARMOUR, mons->ID, it->ID, offsetof (struct WoW, heads[i])}});
		return 1;
	}
	return 0;
}

int mons_can_takeoff (struct Monster *mons, struct Item *it)
{
	return 1;
}

int mons_try_takeoff (struct Monster *mons, struct Item *it)
{
	if (it_no(it))
		return 0;
	ev_queue (0, (union Event) { .mtakeoff_armour = {EV_MTAKEOFF_ARMOUR, mons->ID, it->ID}});
	return 1;
}

void mons_try_hit (struct Monster *mons, int y, int x)
{
	int delay = mons->speed;
	int arm = 0;
	ev_queue (delay, (union Event) { .mdohitm = {EV_MDOHITM, mons->ID}});
	mons_start_hit (mons, y, x, arm, curtick + delay);
}

void mons_try_hitm (struct Monster *mons, int y, int x)
{
	struct Monster *to = dlv_mvmons (mons->dlevel, mons->zloc, mons->yloc+y, mons->xloc+x);
	struct MStatus *s = &to->status;
	if (s->moving.arrival == 0 ||
		s->moving.arrival >= curtick + mons->speed)
	{
		mons_try_hit (mons, y, x);
		return;
	}
	int Y = s->moving.ydir, X = s->moving.xdir;
	if (abs (Y+y) > 1 || abs (X+x) > 1 ||
		dlv_mvmonsID (mons->dlevel, mons->zloc, mons->yloc+y+Y, mons->xloc+x+X))
	{
		mons_try_hit (mons, y, x);
		return;
	}
	mons_try_hit (mons, Y+y, X+x);
}

int mons_can_move (struct Monster *mons, int y, int x)
{
	if (y < -1 || y > 1 || x < -1 || x > 1)
		return 0;
	struct DLevel *lvl = dlv_lvl (mons->dlevel);
	int yloc = mons->yloc + y, xloc = mons->xloc + x;
	if (yloc < 0 || yloc >= lvl->h ||
	    xloc < 0 || xloc >= lvl->w)
		return 0;
	int n = dlv_index (lvl, mons->zloc, yloc, xloc);
	if (lvl->monsIDs[n])
		return 0;
	return map_bpassable (lvl, n);
}

void mons_try_move (struct Monster *mons, int y, int x)
{
	int delay = mons->status.flashing.end ? mons->status.flashing.speed : mons->speed;
	ev_queue (delay, (union Event) { .mdomove = {EV_MDOMOVE, mons->ID}});
	mons_start_move (mons, y, x, curtick + delay);
}

void mons_try_wait (struct Monster *mons)
{
	ev_queue (mons->speed, (union Event) { .mpoll = {EV_MPOLL, mons->ID}});
}

void mons_try_wield (struct Monster *mons, struct Item *item)
{
	ev_queue (mons->speed, (union Event) { .mwield = {EV_MWIELD, mons->ID, 0, item->ID}});
}

void mons_tilefrost (struct Monster *mons, int zloc, int yloc, int xloc)
{
	struct DLevel *lvl = dlv_lvl (mons->dlevel);
	int n = dlv_index (lvl, zloc, yloc, xloc);
	if (lvl->monsIDs[n])
	{//TODO anger monster
	}
	int i;
	Vector items = lvl->items[n];
	for (i = 0; i < items->len; ++ i)
	{
		struct Item *it = v_at (items, i);
		if (!it_freeze (it))
		{
			/* it has disappeared */
			-- i;
		}
	}
}

void mons_wield (struct Monster *mons, int arm, struct Item *it)
{
	item_put (it, (union ItemLoc) { .wield = {LOC_WIELDED, mons->ID, it->loc.inv.invnum, arm}});
}

void mons_unwield (struct Monster *mons, int arm)
{
	struct Item *it = mons->wearing.weaps[arm];
	item_put (it, (union ItemLoc) { .inv = {LOC_INV, mons->ID, it->loc.wield.invnum}});
}

void mons_wear (struct Monster *mons, struct Item *item, size_t offset)
{
	*(struct Item **)((void*)&mons->wearing + offset) = item;
	it_wear (item, offset);
	mons->armour += it_def (item);
}

void mons_take_off (struct Monster *mons, struct Item *item)
{
	*(struct Item **)((char*)&mons->wearing + it_worn_offset (item)) = 0;
	it_unwear (item);
	mons->armour -= it_def (item);
}

void mons_try_evade (struct Monster *mons, int y, int x)
{
	int delay = mons->speed/3;
	ev_queue (delay, (union Event) { .mdoevade = {EV_MDOEVADE, mons->ID}});
	mons_start_evade (mons, y, x, curtick + delay, curtick + delay + mons->speed);
}

void draw_map_mons (struct Monster *mons)
{
	draw_map_xyz (dlv_lvl (mons->dlevel), mons->zloc, mons->yloc, mons->xloc);
}

void mons_start_evade (struct Monster *mons, int y, int x, Tick arrival, Tick recovery)
{
	mons->status.evading = (typeof(mons->status.evading)) {y, x, arrival, recovery};
	draw_map_mons (mons);
}

void mons_stop_evade (struct Monster *mons)
{
	mons->status.evading = (typeof(mons->status.evading)) {0, 0, 0, 0};
	draw_map_mons (mons);
}

void mons_start_move (struct Monster *mons, int y, int x, Tick arrival)
{
	mons->status.moving = (typeof(mons->status.moving)) {y, x, arrival};
	draw_map_mons (mons);
}

void mons_stop_move (struct Monster *mons)
{
	mons->status.moving = (typeof(mons->status.moving)) {0, 0, 0};
	draw_map_mons (mons);
}

void mons_start_hit (struct Monster *mons, int y, int x, int arm, Tick arrival)
{
	eff_mons_starts_hit (mons, y, x, arrival);
	mons->status.attacking = (typeof(mons->status.attacking)) {y, x, arm, arrival};
	draw_map_mons (mons);
}

void mons_stop_hit (struct Monster *mons)
{
	mons->status.attacking = (typeof(mons->status.attacking)) {0, 0, -1, 0};
	draw_map_mons (mons);
}

int mons_take_damage (struct Monster *mons, struct Monster *fr, int dmg, enum DMG_TYPE type)
{
	mons->HP -= dmg;
	if (mons->HP <= 0)
	{
		mons->HP = 0;
		if (type == DTYP_BLEED && !fr)
			p_msg ("The %s bleeds out and dies!", mons_typename (mons));
		mons_kill (fr, mons);
		return 0;
	}
	if (type == DTYP_CUT && mons_can_bleed (mons))
		mons_startbleed (mons);
	return 1;
}

int mons_can_bleed (const struct Monster *mons)
{
	return mons->mflags&FL_FLSH;
}

void mons_startbleed (struct Monster *mons)
{
	if (mons->status.bleeding)
		return;
	mons->status.bleeding = 1;
	ev_queue (1000, (union Event) { .mbleed = {EV_MBLEED, mons->ID}});
}

void mons_stopbleed (struct Monster *mons)
{
	mons->status.bleeding = 0;
}

void mons_kill (struct Monster *fr, struct Monster *to)
{
	if (fr)
		eff_mons_kills_mons (fr, to);
	if (mons_isplayer(to))
	{
		p_msgbox ("You die...");
		U.playing = PLAYER_LOSTGAME;
		rem_mid (to->ID);
		return;
	}
	int exp_gain = to->exp;
	mons_dead (to);
	mons_get_exp (fr, exp_gain);
}

void mons_dead (struct Monster *mons)
{
	if (!mons->pack)
		goto mcorpse;
	int i;
	for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
	{
		struct Item *packitem = &mons->pack->items[i];
		if (it_no(packitem))
			continue;
		item_put (packitem, it_monsdloc (mons));
	}
	free (mons->pack);

mcorpse: ;
	/* item drops */
	if (onein (5))
		item_gen (it_monsdloc (mons));
	/* add corpse */
	struct Item corpse;
	mons_corpse (mons, &corpse);
	struct Item *ret = item_put (&corpse, it_monsdloc (mons));
	ev_queue (50000 + rn(10000), (union Event) { .itrot = {EV_ITROT, ret->ID}});

	/* remove dead monster */
	rem_mid (mons->ID);
}

void mons_anger (struct Monster *fr, struct Monster *to)
{
	if (mons_isplayer (to))
		return;
	if (to->ctr.mode == CTR_AI_TIMID)
		eff_mons_angers_mons (fr, to);
	to->ctr.mode = CTR_AI_AGGRO;
	to->ctr.aggro.ID = fr->ID;
}

void mons_calm (struct Monster *mons)
{
	mons->ctr.mode = CTR_AI_TIMID;
	eff_mons_calms (mons);
}

void mons_poll (struct Monster *th)
{
	if (!is (th))
		return;
	struct MStatus *st = &th->status;
	if (st->moving.arrival || st->attacking.arrival || st->helpless || st->evading.finish)
		return;
	switch (th->ctr.mode)
	{
	case CTR_NONE:
		/* should never happen */
		break;
	case CTR_PL:
		/* player in normal mode */
		pl_poll (th);
		return;
	case CTR_PL_CONT:
	{
		/* player in continuation mode - attempt to call the continuation */
		int res = ((th->ctr.cont.cont) (th));
		if (res <= 0)
			th->ctr.mode = CTR_PL;
		if (res != -1)
			return;
	}
	case CTR_PL_FOCUS:
		/* player in focus mode */
		pl_poll (th);
		return;
#ifdef SIM
	case CTR_AI_SIM_FARMER:
#endif /* SIM */
	case CTR_AI_TIMID:
		/* calm non-player monster */
		AI_TIMID_poll (th);
		return;
	case CTR_AI_HOSTILE:
		/* hostile monster */
		AI_HOSTILE_poll (th);
		return;
	case CTR_AI_AGGRO:
		/* aggravated monster */
		AI_AGGRO_poll (th);
		return;
	}
	panic ("End of mons_poll reached.");
	return;
}

Tick mons_tregen (struct Monster *th)
{
	return 1000;
}

int mons_throwspeed (struct Monster *mons, struct Item *it)
{
	return rn(3) + mons->str/2 - it_weight(it)/500;
}

int proj_hitm (struct Item *proj, struct Monster *to)
{
	return 1;
}

int proj_hitdmg (struct Item *proj, struct Monster *to)
{
	// TODO mitigated by armour?
	return it_projdamage (proj);
}

int mons_skill (const struct Monster *from, const struct Item *with)
{
	int i;
	enum SK_TYPE type = it_skill (with);
	if (!from->skills)
		return 0;
	for (i = 0; i < from->skills->len; ++ i)
	{
		struct Skill *sk = v_at (from->skills, i);
		if (sk->type != type)
			continue;
		return sk->level;
	}
	return 0;
}

int mons_attk_bonus (const struct Monster *mons, const struct Item *with)
{
	int level = mons_skill (mons, with);
	return (level*(level+1))/2;
}

void mons_ex_skill (struct Monster *mons, Skill sk)
{
	if (!mons)
		return;
	if (!mons_gets_exp (mons))
		return;
	sk->exp ++;
	int level = mons_level (sk->exp);
	if (level != sk->level)
	{
		sk->level = level;
		eff_mons_sk_levels_up (mons, sk);
	}
}

void mons_exercise (struct Monster *from, struct Item *with)
{
	int i;
	enum SK_TYPE type = it_skill (with);
	if (type == SK_NONE)
		return;
	struct Skill *sk = NULL;
	if (!from->skills)
		panic ("monster has no skill slots in mons_exercise");
	for (i = 0; i < from->skills->len; ++ i)
	{
		sk = v_at (from->skills, i);
		if (sk->type == type)
			break;
	}
	if (i >= from->skills->len)
	{
		struct Skill add = {type, 0, 0};
		sk = v_push (from->skills, &add);
	}
	mons_ex_skill (from, sk);
}

int mons_hitm (const struct Monster *from, const struct Monster *to, const struct Item *with)
{
	if ((to->status.defending.ydir || to->status.defending.xdir) &&
	    to->status.defending.ydir + to->yloc == from->yloc &&
	    to->status.defending.xdir + to->xloc == from->xloc)
		return 0;
	return rn(20) && (onein (20) || (5 + mons_skill (from, with) >= rn(to->armour*2 + 7)));
}

int mons_hitdmg (const struct Monster *from, const struct Monster *to, const struct Item *with)
{
	if (!with)
		return (rn(from->str/6 + 2) + rn((from->str+3)/6 + 2));
	int attk = it_attk (with) + mons_attk_bonus (from, with);
	int dmg = (rn(1 + attk/2) + rn(1 + (attk+1)/2));
	return dmg;
}

int mons_ST_hit (struct Monster *from, struct Item *with)
{
	if (!with)
		return rn(3);
	return 3 + it_weight(with)/500;
}

int mons_HP_regen (struct Monster *th)
{
	return onein (8) * (rn(th->str) >= 4);
}

int mons_ST_regen (struct Monster *th)
{
	return rn(1+th->con/2);
}

int mons_MP_regen (struct Monster *th)
{
	return onein (10) * rn(1 + (th->wis + rn(4))/4);
}

void mons_stats_changed (struct Monster *mons)
{
	/* HP */
	int HP_max = mons_get_HP (mons);
	mons->HP = (mons->HP * HP_max) / mons->HP_max;
	mons->HP_max = HP_max;
	if (mons->HP > mons->HP_max)
		mons->HP = mons->HP_max;
	/* ST */
	int ST_max = mons_get_ST (mons);
	mons->ST = (mons->ST * ST_max) / mons->ST_max;
	mons->ST_max = ST_max;
	if (mons->ST > mons->ST_max)
		mons->ST = mons->ST_max;
	/* MP */
	int MP_max = mons_get_MP (mons);
	mons->MP = (mons->MP * MP_max) / mons->MP_max;
	mons->MP_max = MP_max;
	if (mons->MP > mons->MP_max)
		mons->MP = mons->MP_max;
}

void mons_get_exp (struct Monster *mons, int exp)
{
	if (!mons)
		return;
	if (!mons_gets_exp (mons))
		return;
	mons->exp += exp;
	int new_level = mons_level (mons->exp);
	if (new_level == mons->level)
		return;
	if (new_level < mons->level)
	{
		panic ("monster leveled down in mons_get_exp");
		return;
	}
	mons->level ++;
	if (mons->level < mons_level (mons->exp))
		mons->exp = explevel[mons->level]-1;
	eff_mons_levels_up (mons);
	/* stats */
	mons->str ++;
	mons->con ++;
	mons->wis ++;
	mons->agi ++;
	//pl_choose_attr_gain (mons, 1);
	mons_stats_changed (mons);
}

int mons_cont (struct Monster *player, MCont cont, union ContData *data)
{
	if (player->ctr.mode != CTR_PL)
		panic ("mons_cont called on non-CTR_PL monster!");
	player->ctr.cont.mode = CTR_PL_CONT;
	player->ctr.cont.cont = cont;
	if (data)
		memcpy (&player->ctr.cont.data, data, sizeof(*data));
	return 1;
}

int mons_isplayer (struct Monster *th)
{
	if (!th)
		return 0;
	switch (th->ctr.mode)
	{
	case CTR_NONE:
		panic ("CTR_NONE player\n");
		return 0;
	case CTR_PL:
	case CTR_PL_CONT:
	case CTR_PL_FOCUS:
		return 1;
	case CTR_AI_TIMID:
	case CTR_AI_HOSTILE:
	case CTR_AI_AGGRO:
	default:
		return 0;
	}
}

const char *mons_typename (struct Monster *mons)
{
	return mons->mname;
}

int AI_weapcmp (struct Monster *ai, struct Item *w1, struct Item *w2)
{
	if (it_no(w2))
		return !it_no(w1);
	if (it_no(w1))
		return -1;
	return ((uintptr_t)w1) > ((uintptr_t)w2);
}

void AI_TIMID_poll (struct Monster *ai)
{
	int y = rn(3)-1, x = rn(3)-1;
	if (mons_can_move (ai, y, x))
		mons_try_move (ai, y, x);
	else
		mons_try_wait (ai);
}

void AI_HOSTILE_poll (struct Monster *ai)
{
	return AI_TIMID_poll (ai);
	//if (cur_dlevel->player_dist[map_buffer(ai->yloc, ai->xloc)] == -1)
	//	return mons_try_wait (ai);
	/*int y, x, min = 999999, nmin = 0;
	for (y = -1; y <= 1; ++ y) for (x = -1; x <= 1; ++ x)
	{
		if (y == 0 && x == 0)
			continue;
		int a = cur_dlevel->player_dist[map_buffer(ai->yloc + y, ai->xloc + x)];
		if (a == -1)
			continue;
		if (a < min)
		{
			min = a;
			nmin = 1;
		}
		else if (a == min)
			++ nmin;
	}
	if (!nmin)
	{
		y = rn(3)-1; x = rn(3)-1;
		if (mons_can_move (ai, y, x) && onein (3))
			mons_try_move (ai, y, x);
		else
			mons_try_wait (ai);
		return;
	}
	int ch = rn(nmin) + 1;
	for (y = -1; y <= 1; ++ y) for (x = -1; x <= 1; ++ x)
	{
		if (y == 0 && x == 0)
			continue;
		int a = cur_dlevel->player_dist[map_buffer(ai->yloc + y, ai->xloc + x)];
		if (a == min)
			-- ch;
		if (!ch)
		{
			if (mons_can_move (ai, y, x))
				mons_try_move (ai, y, x);
			else if (min == 0)
				mons_try_hitm (ai, y, x);
			else
				mons_try_wait (ai);
			return;
		}
	}
	panic("end of AI_HOSTILE_poll reached");*/
}

void AI_AGGRO_poll (struct Monster *ai)
{
	struct Monster *to = MTHIID (ai->ctr.aggro.ID);
	if (!to)
	{
		mons_calm (ai);
		mons_poll (ai);
		return;
	}
/*
	if (!ai->pack)
		goto skip_weapon;
	/ * if you have a better weapon then wield it * /
	struct Item *curweap = ai->wearing.weaps[0];
	struct Item *bestweap = curweap;
	int i;
	/ * find the best weapon in the monster's inventory, defaulting to the
	 * currently wielded weapon * /
	for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
	{
		struct Item *item = &ai->pack->items[i];
		if (it_no(item))
			continue;
		if (AI_weapcmp (ai, item, bestweap) > 0)
			bestweap = item;
	}
	if (bestweap != curweap)
	{
		mons_try_wield (ai, bestweap);
		return;
	}

skip_weapon: ;
	/ * where you are * /
	int aiy = ai->yloc, aix = ai->xloc;

	/ * move randomly if you can't see your target * /
	if (!bres_draw (aiy, aix, to->yloc, to->xloc, map_graph->w, NULL, dlv_attr(ai->dlevel), NULL))
	{
		int y = rn(3) - 1, x = rn(3) - 1;
		if (mons_can_move (ai, y, x))
			mons_try_move (ai, y, x);
		else
			mons_try_wait (ai);
		return;
	}

	/ * otherwise you can see your target; this is where you aim to travel * /
	int toy = to->yloc + to->status.moving.ydir;
	int tox = to->xloc + to->status.moving.xdir;

	/ * don't aim at yourself; TODO better movement prediction *
	if (toy == aiy && tox == aix)
	{
		toy = to->yloc;
		tox = to->xloc;
	}

	/ * the direction you should go in *
	int y = (aiy < toy) - (aiy > toy);
	int x = (aix < tox) - (aix > tox);

	/ * attack if that is where your target will be *
	if (aiy + y == toy && aix + x == tox)
		mons_try_hit (ai, y, x);
	/ * move towards your target if you can *
	else if (mons_can_move (ai, y, x))
		mons_try_move (ai, y, x);
	/ * otherwise try similar directions *
	else if (mons_can_move (ai, 0, x))
		mons_try_move (ai, 0, x);
	else if (mons_can_move (ai, y, 0))
		mons_try_move (ai, y, 0);
	/ * give up *
	else
		mons_try_wait (ai); */
}

