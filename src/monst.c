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
#include "include/map.h"

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
	int level = mons_level (cur_dlevel->playerIDs->data[0]);
	uint32_t p_exp = level < maxlevel ? explevel[level] : explevel[maxlevel-1]*2;
	uint32_t total_weight = 0;

	for (i = 0; i < MTYP_NUM_MONS; ++i)
	{
		if (nogen(i)) continue; /* genocided or unique and generated etc */
		array[i] = expcmp(p_exp, mons_types[i].exp);
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

int mons_gets_exp (MonsID mons)
{
	if (!mons)
		return 0;
	return mons_isplayer (mons);
}

int mons_is (MonsID mons)
{
	return mons && all_mons->data[mons].ID == mons;
}

struct Monster_internal *mons_internal (MonsID mons)
{
	return &all_mons->data[mons];
}

void mons_getloc (MonsID mons, int *d, int *z, int *y, int *x)
{
	struct Monster_internal *mi = mons_internal (mons);
	*d = mi->dlevel;
	*z = mi->zloc;
	*y = mi->yloc;
	*x = mi->xloc;
}

int mons_speed (MonsID mons)
{
	return mons_internal (mons)->speed;
}

struct DLevel *mons_dlv (MonsID mons)
{
	return dlv_lvl (mons_internal (mons)->dlevel);
}

int mons_dlevel (MonsID mons)
{
	return mons_internal (mons)->dlevel;
}

int mons_get_level (MonsID mons)
{
	return mons_internal (mons)->level;
}

int mons_exp (MonsID mons)
{
	return mons_internal (mons)->exp;
}

struct Pack *mons_pack (MonsID mons)
{
	return &mons_internal (mons)->pack;
}

CTR_MODE mons_ctrl (MonsID mons)
{
	return mons_internal (mons)->ctr.mode;
}

int mons_str (MonsID mons)
{
	return mons_internal (mons)->str;
}

int mons_con (MonsID mons)
{
	return mons_internal (mons)->con;
}

int mons_wis (MonsID mons)
{
	return mons_internal (mons)->wis;
}

int mons_armour (MonsID mons)
{
	return mons_internal (mons)->armour;
}

glyph mons_gl  (MonsID mons)
{
	return '@';
}

int mons_charging (MonsID mons)
{
	return 0;
}

int mons_bleeding (MonsID mons)
{
	return mons_internal (mons)->status.bleed.evID;
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

void mons_corpse (MonsID mons, struct Item_internal *item)
{
	*item = new_item (ITYP_NUM_ITEMS + mons_type (mons));
}

enum MTYPE mons_type (MonsID mons)
{
	if (!mons)
		panic ("NULL mi in mons_type");
	struct Monster_internal *mi = mons_internal (mons);
	return mi->mtype;
}

int mons_get_HP (MonsID mons)
{
	struct Monster_internal *mi = mons_internal (mons);
	return 2*mi->str + 1;
}

int mons_get_ST (MonsID mons)
{
	struct Monster_internal *mi = mons_internal (mons);
	if (mi->con <= 5)
		return (3*mi->con)/2 + 5;
	return (4*mi->con) - 8;
}

int mons_get_MP (MonsID mons)
{
	struct Monster_internal *mi = mons_internal (mons);
	return mi->wis + 1;
}

int mons_index (MonsID mons)
{
	struct Monster_internal *mi = mons_internal (mons);
	struct DLevel *lvl = dlv_lvl (mi->dlevel);
	return dlv_index (lvl, mi->zloc, mi->yloc, mi->xloc);
}

int mons_can_wear (MonsID mons, ItemID it, size_t offset)
{
	struct Monster_internal *mi = mons_internal (mons);
	if (it_canwear (it, MONS_HAND))
		return (offset >= offsetof (struct WoW, hands[0]) &&
			offset < offsetof (struct WoW, hands[mi->wearing.narms]));
	if (it_canwear (it, MONS_TORSO))
		return (offset >= offsetof (struct WoW, torsos[0]) &&
			offset < offsetof (struct WoW, torsos[mi->wearing.ntorsos]));
	if (it_canwear (it, MONS_HEAD))
		return (offset >= offsetof (struct WoW, heads[0]) &&
			offset < offsetof (struct WoW, heads[mi->wearing.nheads]));
	return 0;
}

// should be in player
int mons_try_wear (MonsID mons, ItemID item)
{
	int i;
	struct Monster_internal *mi = mons_internal (mons);
	if (it_canwear (item, MONS_HAND))
	{
		for (i = 0; i < mi->wearing.narms; ++ i)
		{
			if (!mi->wearing.hands[i])
				break;
		}
		if (i >= mi->wearing.narms)
		{
			p_msg ("You don't have enough hands!");
			return 0;
		}
		ev_queue (0, mwear_armour, mons, item, offsetof (struct WoW, hands[i]));
		return 1;
	}
	if (it_canwear (item, MONS_TORSO))
	{
		for (i = 0; i < mi->wearing.ntorsos; ++ i)
		{
			if (!mi->wearing.torsos[i])
				break;
		}
		if (i >= mi->wearing.ntorsos)
		{
			p_msg ("You don't have enough torsos!");
			return 0;
		}
		ev_queue (0, mwear_armour, mons, item, offsetof (struct WoW, torsos[i]));
		return 1;
	}
	if (it_canwear (item, MONS_HEAD))
	{
		for (i = 0; i < mi->wearing.nheads; ++ i)
		{
			if (!mi->wearing.heads[i])
				break;
		}
		if (i >= mi->wearing.nheads)
		{
			p_msg ("You don't have enough heads!");
			return 0;
		}
		ev_queue (0, mwear_armour, mons, item, offsetof (struct WoW, heads[i]));
		return 1;
	}
	return 0;
}

int mons_can_takeoff (MonsID mons, ItemID item)
{
	return 1;
}

int mons_try_takeoff (MonsID mons, ItemID item)
{
	ev_queue (0, mtakeoff_armour, mons, item);
	return 1;
}

void mons_try_hit (MonsID mons, int y, int x)
{
	//struct Monster_internal *mi = mons_internal (mons);
	int delay = mons_speed (mons);
	int arm = 0;
	ev_queue (delay, mdohit, mons, arm, 0, y, x);
	//mons_start_hit (mons, y, x, arm, curtick + delay);
	eff_mons_starts_hit (mons, y, x, curtick + delay);
}

void mons_try_hitm (MonsID mons, int y, int x)
{
	/*struct Monster_internal *mi = mons_internal (mons);
	MonsID to = dlv_mvmons (mi->dlevel, mi->zloc, mi->yloc+y, mi->xloc+x);
	struct MStatus *s = &to->status;
	if (s->moving.arrival == 0 ||
		s->moving.arrival >= curtick + mi->speed)
	{
		mons_try_hit (mi, y, x);
		return;
	}
	int Y = s->moving.ydir, X = s->moving.xdir;
	if (abs (Y+y) > 1 || abs (X+x) > 1 ||
		dlv_mvmonsID (mi->dlevel, mi->zloc, mi->yloc+y+Y, mi->xloc+x+X))
	{
		mons_try_hit (mi, y, x);
		return;
	}
	mons_try_hit (mi, Y+y, X+x);*/ // TODO
	mons_try_hit (mons, y, x);
}

int mons_can_move (MonsID mons, int z, int y, int x)
{
	if (z < -1 || z > 1 || y < -1 || y > 1 || x < -1 || x > 1)
		return 0;
	int md, mz, my, mx;
	mons_getloc (mons, &md, &mz, &my, &mx);
	struct DLevel *lvl = mons_dlv (mons);
	int zloc = mz + z, yloc = my + y, xloc = mx + x;
	if (zloc < 0 || zloc >= lvl->t ||
		yloc < 0 || yloc >= lvl->h ||
	    xloc < 0 || xloc >= lvl->w)
		return 0;
	int n = dlv_index (lvl, zloc, yloc, xloc);
	int m = dlv_index (lvl, zloc-1, yloc, xloc);
	if (lvl->monsIDs[n])
		return 0;
	return map_bpassable (lvl, n) && !map_bpassable (lvl, m);
}

int mons_getmovedelay (MonsID mons)
{
	struct Monster_internal *mi = mons_internal (mons);
	return /*mi->status.flashing.end ? mi->status.flashing.speed :*/ mi->speed;
}

void mons_try_move (MonsID mons, int z, int y, int x)
{
	int delay = mons_getmovedelay (mons);
	ev_queue (delay, mdomove, mons, z, y, x);
	//mons_start_move (mons, z, y, x, curtick + delay);
}

void mons_try_wait (MonsID mons)
{
	ev_queue (mons_speed (mons), mpoll, mons);
}

void mons_try_wield (MonsID mons, ItemID item)
{
	ev_queue (mons_speed (mons)/2, mwield, mons, 0, item);
}

void mons_tilefrost (MonsID mons, int zloc, int yloc, int xloc)
{
	struct DLevel *lvl = mons_dlv (mons);
	int n = dlv_index (lvl, zloc, yloc, xloc);
	if (lvl->monsIDs[n])
	{//TODO anger monster
	}
	int i;
	V_ItemID items = lvl->itemIDs[n];
	for (i = 0; i < items->len; ++ i)
	{
		ItemID item = items->data[i];
		if (!it_freeze (item))
		{
			/* it has disappeared */
			-- i;
		}
	}
}

ItemID mons_getweap (MonsID mons, int arm)
{
	return mons_internal (mons)->wearing.weaps[arm];
}

void mons_setweap (MonsID mons, int arm, ItemID item)
{
	mons_internal (mons)->wearing.weaps[arm] = item;
}

void mons_wield (MonsID mons, int arm, ItemID item)
{
	struct ItemInInv inv;
	if (!it_inv (item, &inv))
		return; // what??
	item_put (item, (union ItemLoc) { .wield = {LOC_WIELDED, mons, inv.invnum, arm}});
}

void mons_unwield (MonsID mons, int arm)
{
	struct ItemWielded wield;
	ItemID item = mons_getweap (mons, arm);
	if (!it_wield (item, &wield))
		return; // what??
	item_put (item, (union ItemLoc) { .inv = {LOC_INV, mons, wield.invnum}});
}

void mons_wear (MonsID mons, ItemID item, size_t offset)
{
	struct Monster_internal *mi = mons_internal (mons);
	*(ItemID *)((void*)&mi->wearing + offset) = item;
	it_wear (item, offset);
	mi->armour += it_def (item);
}

void mons_take_off (MonsID mons, ItemID item)
{
	struct Monster_internal *mi = mons_internal (mons);
	*(ItemID *)((char*)&mi->wearing + it_worn_offset (item)) = 0;
	it_unwear (item);
	mi->armour -= it_def (item);
} // TODO wearing properly

void mons_try_evade (MonsID mons, int y, int x)
{
	int delay = mons_speed (mons)/3;
	ev_queue (delay, mdoevade, mons, 0, y, x);
	//mons_start_evade (mi, y, x, curtick + delay, curtick + delay + mons_speed (mons));
}
/*
void mons_start_hit (MonsID mons, int y, int x, int arm, Tick arrival)
{
	//eff_mons_starts_hit (mi, y, x, arrival);
	//mi->status.attacking = (typeof(mi->status.attacking)) {y, x, arm, arrival};
	//draw_map_mons (mi);
}

void mons_stop_hit (MonsID mons)
{
	//mi->status.attacking = (typeof(mi->status.attacking)) {0, 0, -1, 0};
	//draw_map_mons (mi);
}*/

int mons_take_damage (MonsID mons, MonsID fr, int dmg, enum DMG_TYPE type)
{
	struct Monster_internal *mi = mons_internal (mons);
	mi->HP -= dmg;
	if (mi->HP <= 0)
	{
		mi->HP = 0;
		if (type == DTYP_BLEED && !fr)
			p_msg ("The %s bleeds out and dies!", mons_typename (mons));
		mons_kill (fr, mons);
		return 0;
	}
	if (type == DTYP_CUT && mons_can_bleed (mons))
		mons_startbleed (mons);
	return 1;
}

int mons_can_bleed (MonsID mons)
{
	return mons_internal (mons)->mflags & FL_FLSH;
}

void mons_startbleed (MonsID mons)
{
	//if (mi->status.bleeding)
	//	return;
	//mi->status.bleeding = 1;
	ev_queue (1000, mbleed, mons);
}

void mons_stopbleed (MonsID mons)
{
	//mi->status.bleeding = 0;
}

void mons_kill (MonsID fr, MonsID to)
{
	if (fr)
		eff_mons_kills_mons (fr, to);
	if (mons_isplayer(to))
	{
		p_msgbox ("You die...");
		U.playing = PLAYER_LOSTGAME;
		mons_destroy (to);
		return;
	}
	int exp_gain = mons_exp (to);
	mons_dead (to);
	mons_get_exp (fr, exp_gain);
}

void mons_dead (MonsID mons)
{
	int i;
	struct Pack *p = mons_pack (mons);
	for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
	{
		ItemID item = p->items[i];
		if (!item)
			continue;
		item_put (item, it_monsdloc (mons));
	}

	/* item drops */
	if (onein (5))
		item_gen (it_monsdloc (mons));
	/* add corpse */
	struct Item_internal corpse;
	mons_corpse (mons, &corpse);
	ItemID ret = item_create (&corpse, it_monsdloc (mons));
	ev_queue (50000 + rn(10000), itrot, ret);

	/* remove dead monster */
	mons_destroy (mons);
}

void mons_anger (MonsID fr, MonsID to)
{
	if (mons_isplayer (to))
		return;
	struct Monster_internal *mi = mons_internal (to);
	if (mi->ctr.mode == CTR_AI_TIMID)
		eff_mons_angers_mons (fr, to);
	mi->ctr.mode = CTR_AI_AGGRO;
	mi->ctr.aggro.ID = fr;
}

void mons_calm (MonsID mons)
{
	struct Monster_internal *mi = mons_internal (mons);
	mi->ctr.mode = CTR_AI_TIMID;
	eff_mons_calms (mons);
}

void mons_poll (MonsID mons) // TODO bail if mons busy!
{
	struct MStatus *st = &mons_internal(mons)->status;
	if (st->move.evID || st->hit.evID || /*st->helpless ||*/ st->evade.evID || st->shield.evID)
		return;
	switch (mons_ctrl (mons))
	{
	case CTR_NONE:
		/* should never happen */
		break;
	case CTR_PL:
		/* player in normal mode */
		pl_poll (mons);
		return;
#ifdef SIM
	case CTR_AI_SIM_FARMER:
#endif /* SIM */
	case CTR_AI_TIMID:
		/* calm non-player monster */
		AI_TIMID_poll (mons);
		return;
	case CTR_AI_HOSTILE:
		/* hostile monster */
		AI_HOSTILE_poll (mons);
		return;
	case CTR_AI_AGGRO:
		/* aggravated monster */
		AI_AGGRO_poll (mons);
		return;
	}
	panic ("End of mons_poll reached.");
	return;
}

Tick mons_tregen (MonsID th)
{
	return 1000;
}

int mons_throwspeed (MonsID mons, ItemID item)
{
	return rn(3) + mons_str (mons)/2 - it_weight(item)/500;
}

int proj_hitm (ItemID proj, MonsID to)
{
	return 1;
}

int proj_hitdmg (ItemID proj, MonsID to)
{
	// TODO mitigated by armour?
	return it_projdamage (proj);
}

int mons_skill (MonsID from, ItemID with)
{
	//int i;
	//enum SK_TYPE type = it_skill (with);
	/*if (!from->skills)
		return 0;
	for (i = 0; i < from->skills->len; ++ i)
	{
		struct Skill *sk = v_at (from->skills, i);
		if (sk->type != type)
			continue;
		return sk->level;
	}*/
	return 0;
}

Vector mons_skills (MonsID mons)
{
	return mons_internal(mons)->skills;
}

int mons_attk_bonus (MonsID mons, ItemID with)
{
	int level = mons_skill (mons, with);
	return (level*(level+1))/2;
}

void mons_ex_skill (MonsID mons, Skill sk)
{
	/*if (!mi)
		return;
	if (!mons_gets_exp (mi))
		return;
	sk->exp ++;
	int level = mons_level (sk->exp);
	if (level != sk->level)
	{
		sk->level = level;
		eff_mons_sk_levels_up (mi, sk);
	}*/
}

void mons_exercise (MonsID from, ItemID with)
{
	/*int i;
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
	mons_ex_skill (from, sk);*/
}

int mons_hitm (MonsID from, MonsID to, ItemID with)
{
	int d, z, y, x;
	mons_getloc (from, &d, &z, &y, &x);
	struct Monster_internal *mi = mons_internal (to);
	if ((mi->status.shield.ydir || mi->status.shield.xdir) &&
	    mi->status.shield.ydir + mi->yloc == y &&
	    mi->status.shield.xdir + mi->xloc == x)
		return 0;
	return rn(20) && (onein (20) || (5 + mons_skill (from, with) >= rn(mons_armour (to)*2 + 7)));
}

int mons_hitdmg (MonsID from, MonsID to, ItemID with)
{
	if (!with)
		return (rn(mons_str (from)/6 + 2) + rn((mons_str (from)+3)/6 + 2));
	int attk = it_attk (with) + mons_attk_bonus (from, with);
	int dmg = (rn(1 + attk/2) + rn(1 + (attk+1)/2));
	return dmg;
}

int mons_ST_hit (MonsID from, ItemID with)
{
	if (!with)
		return rn(3);
	return 3 + it_weight(with)/500;
}

int mons_HP_regen (MonsID mons)
{
	return onein (8) * (rn(mons_str (mons)) >= 4);
}

int mons_ST_regen (MonsID mons)
{
	return rn(1+mons_con (mons)/2);
}

int mons_MP_regen (MonsID mons)
{
	return onein (10) * rn(1 + (mons_wis (mons) + rn(4))/4);
}

void mons_stats_changed (MonsID mons)
{
	struct Monster_internal *mi = mons_internal (mons);
	/* HP */
	int HP_max = mons_get_HP (mons);
	mi->HP = (mi->HP * HP_max) / mi->HP_max;
	mi->HP_max = HP_max;
	if (mi->HP > mi->HP_max)
		mi->HP = mi->HP_max;
	/* ST */
	int ST_max = mons_get_ST (mons);
	mi->ST = (mi->ST * ST_max) / mi->ST_max;
	mi->ST_max = ST_max;
	if (mi->ST > mi->ST_max)
		mi->ST = mi->ST_max;
	/* MP */
	int MP_max = mons_get_MP (mons);
	mi->MP = (mi->MP * MP_max) / mi->MP_max;
	mi->MP_max = MP_max;
	if (mi->MP > mi->MP_max)
		mi->MP = mi->MP_max;
}

void mons_get_exp (MonsID mons, int exp)
{
	if (!mons)
		return;
	if (!mons_gets_exp (mons))
		return;
	struct Monster_internal *mi = mons_internal (mons);
	mi->exp += exp;
	int new_level = mons_level (mi->exp);
	if (new_level == mi->level)
		return;
	if (new_level < mi->level)
	{
		panic ("monster leveled down in mons_get_exp");
		return;
	}
	mi->level ++;
	if (mi->level < mons_level (mi->exp))
		mi->exp = explevel[mi->level]-1;
	eff_mons_levels_up (mons);
	/* stats */
	mi->str ++;
	mi->con ++;
	mi->wis ++;
	mi->agi ++;
	//pl_choose_attr_gain (mi, 1);
	mons_stats_changed (mons);
}

int mons_isplayer (MonsID mons)
{
	struct Monster_internal *mi = mons_internal (mons);
	switch (mi->ctr.mode)
	{
	case CTR_NONE:
		panic ("CTR_NONE player\n");
		return 0;
	case CTR_PL:
		return 1;
	case CTR_AI_TIMID:
	case CTR_AI_HOSTILE:
	case CTR_AI_AGGRO:
	default:
		return 0;
	}
}

const char *mons_typename (MonsID mons)
{
	return mons_internal (mons)->mname;
}

int AI_weapcmp (MonsID ai, ItemID w1, ItemID w2)
{
	//if (it_no(w2))
	//	return !it_no(w1);
	//if (it_no(w1))
	//	return -1;
	return ((uintptr_t)w1) > ((uintptr_t)w2);
}

void AI_TIMID_poll (MonsID ai)
{
	int y = rn(3)-1, x = rn(3)-1;
	if (mons_can_move (ai, 0, y, x))
		mons_try_move (ai, 0, y, x);
	else
		mons_try_wait (ai);
}

void AI_HOSTILE_poll (MonsID ai)
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

void AI_AGGRO_poll (MonsID ai)
{
	//MonsID to = (ai->ctr.aggro.ID);
	//if (!to)
	{
		mons_calm (ai);
		mons_poll (ai);
		return;
	}
/*
	if (!ai->pack)
		goto skip_weapon;
	/ * if you have a better weapon then wield it * /
	ItemID curweap = ai->wearing.weaps[0];
	ItemID bestweap = curweap;
	int i;
	/ * find the best weapon in the monster's inventory, defaulting to the
	 * currently wielded weapon * /
	for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
	{
		ItemID item = &ai->pack->items[i];
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

