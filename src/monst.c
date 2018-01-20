/* monst.c */

#include "include/all.h"
#include "include/thing.h"
#include "include/panel.h"
#include "include/rand.h"
#include "include/drawing.h"
#include "include/dlevel.h"
#include "include/monst.h"
#include "include/player.h"
#include "include/event.h"

#include <stdio.h>
#include <stdbool.h>

struct player_status U;
char *s_hun[] = {
	"Full",
	"",
	"Hungry",
	"Hungry!",
	"Starved",
	"Dead"
};
/*
char *get_hungerstr()
{
	if (U.hunger < HN_LIMIT_1)
		return s_hun[0];
	if (U.hunger < HN_LIMIT_2)
		return s_hun[1];
	if (U.hunger < HN_LIMIT_3)
		return s_hun[2];
	if (U.hunger < HN_LIMIT_4)
		return s_hun[3];
	if (U.hunger < HN_LIMIT_5)
		return s_hun[4];
	return s_hun[5];
}

bool digesting()
{
	return true;
}*/

void setup_U ()
{
	int i;

	U.playing = PLAYER_ERROR;	/* If this function returns early */
//	U.hunger = 100;
	U.luck = 0;

	for (i = 0; i < 6; ++i)
		U.attr[i] = 10;

	U.playing = PLAYER_STARTING;
}

void get_cinfo ()
{
	U.role = 1;
	U.playing = PLAYER_PLAYING;
}

int expcmp (int p_exp, int m_exp)
{
	if (p_exp >= m_exp * 2)
		return 5;
	if ((p_exp * 20) + 20 >= m_exp * 19)
		return 50;
	if ((p_exp * 2) >= m_exp)
		return 1;
	return 0;
}

bool nogen (int mons_id)
{
	if (mons_id == MTYP_SATAN)
		return ((U.m_glflags&MGL_GSAT) != 0);

	return false;
}

int player_gen_type ()
{
	int i, array[NUM_MONS];
	uint32_t p_exp = 20; // TODO
	uint32_t total_weight = 0;

	for (i = 0; i < NUM_MONS; ++i)
	{
		if (nogen(i)) continue; /* genocided or unique and generated etc */
		array[i] = expcmp(p_exp, all_mons[i].exp);
		total_weight += array[i];
	}

	for (i = 0; i < NUM_MONS; ++i)
	{
		if (array[i] > rn(total_weight))
			break;
		total_weight -= array[i];
	}
	if (i < NUM_MONS) return i;

	/* If the player has no exp this will happen */
	return 0;
}

int mons_get_wt (struct Monster *mons)
{
	return CORPSE_WEIGHTS[mons->mflags >> 29];
}

void mons_corpse (struct Monster *mons, Ityp *itype)
{
	/* fill in the data */
	snprintf (itype->name, ITEM_NAME_LENGTH, "%s corpse", mons->mname);
	itype->type = ITYP_CORPSE;
	itype->wt   = mons_get_wt(mons);
	itype->attr = 0;
	itype->gl   = ITCH_CORPSE | (mons->gl & ~0xff);
}

/* Return values:
 * 0 = failed to move; 1 = moved as desired;
 * 2, 3, 4 = did not move as desired but used turn */
int mons_move (struct Monster *th, int y, int x) /* each either -1, 0 or 1 */
{
	if (!(x || y))
		return 0;
	int can = can_amove (get_sqattr (dlv_lvl(th->dlevel), th->yloc + y, th->xloc + x));
	/* like a an unmoveable boulder or something */
	if (!can)
		return 0;
	/* you can and everything's fine, nothing doing */
	else if (can == 1)
	{
		if (mons_isplayer (th))
		{
			pl_queue (th, (union Event) { .mmove = {EV_MMOVE, th->ID, y, x}});
			return pl_execute (th->speed, th, 0);
		}
		ev_queue (0, (union Event) { .mmove = {EV_MMOVE, th->ID, y, x}});
		ev_queue (th->speed+1, (union Event) { .mturn = {EV_MTURN, th->ID}});
		return 1;
	}
	/* melee attack! */
	else if (can == 2)
	{
		if (mons_isplayer (th))
		{
			pl_queue (th, (union Event) { .mattkm = {EV_MATTKM, th->ID, y, x}});
			return pl_execute (th->speed, th, 0);
		}
		ev_queue (0, (union Event) { .mattkm = {EV_MATTKM, th->ID, y, x}});
		ev_queue (th->speed+1, (union Event) { .mturn = {EV_MTURN, th->ID}});
		return 2;
	}
	else if (can == 3)
	{
		ev_queue (0, (union Event) { .mopendoor = {EV_MOPENDOOR, th->ID, y, x}});
		return 3;
	}
	else if (can == 4)
	{
		ev_queue (0, (union Event) { .mclosedoor = {EV_MCLOSEDOOR, th->ID, y, x}});
		return 4;
	}
	/* off map or something */
	else if (can == -1)
	{
		/* nothing to do except return 0 (move not allowed) */
		return 0;
	}
	/* shouldn't get to here -- been a mistake */
	panic ("mons_move() end reached");
	return 0;
}

char escape (char a)
{
	if (a < 0x20)
		return a + 0x40;
	else
		return a;
}

int mons_take_turn (struct Monster *th)
{
	switch (th->ctr.mode)
	{
	case CTR_NONE:
		/* should never happen */
		break;
	case CTR_PL:
		/* player in normal mode */
		return pl_take_turn (th);
	case CTR_PL_CONT:
	{
		/* player in continuation mode - attempt to call the continuation */
		int res = ((th->ctr.cont.cont) (th));
		if (res <= 0)
			th->ctr.mode = CTR_PL;
		if (res != -1)
			return 1;
	}
	case CTR_PL_FOCUS:
		/* player in focus mode */
		return pl_take_turn (th);
	case CTR_AI_TIMID:
		/* calm non-player monster */
		return AI_TIMID_take_turn (th);
	case CTR_AI_HOSTILE:
		/* hostile monster */
		return AI_HOSTILE_take_turn (th);
	case CTR_AI_AGGRO:
		/* aggravated monster */
		return AI_AGGRO_take_turn (th);
	}
	panic ("End of mons_take_turn reached.");
	return -1;
}

/* TODO is it polymorphed? */
bool mons_edible (struct Monster *th, struct Item *item)
{
	return ((item->type.gl & 0xFF) == ITCH_FOOD);
}

Tick mons_tmgen ()
{
	return 10000 + rn (500);
}

Tick mons_tregen (struct Monster *th)
{
	return 1000;
}

void mons_passive_attack (struct Monster *from, struct Monster *to) // ACTION
{
/*	uint32_t t;
	int type = from.type;
	char posv[30];
	for (t = 0; t < A_NUM; ++t)
		if ((mons[type].attacks[t][2] & 0xFFFF) == ATTK_PASS)
			break;
	if (t >= A_NUM)
		return;

	switch (mons[type].attacks[t][2] >> 16)
	{
		case ATYP_ACID:
		{
			w_pos (posv, (char *)mons[type].name, 30);
			if (from == player)
				p_msg ("You splash the %s with acid!", mons[type].name);
			else if (to == player)
				p_msg ("You are splashed by the %s acid!", posv);
		}
	}*/
}

void mons_box (struct Monster *mons, BoxType type)
{
	mons->boxflags |= 1<<type;
}
struct Item *player_use_pack (struct Monster *th, char *msg, uint32_t accepted)
{
	struct Item *It = NULL;
	char in = show_contents (th->pack, accepted, msg);
	//char in, cs[100];
	//bool tried = false;
/*
	do
	{
		if (tried)
			p_msg ("No such item.");
		tried = false;

		pack_get_letters (pmons.pack, cs);
		in = p_ask (cs, msg);
		if (in == '?')
		{
			gr_getch ();
			continue;
		}
		if (in == ' ' || in == 0x1B)
			break;
		if (in == '*')
		{
			show_contents (pmons.pack, ITCAT_ALL, "Inventory");
			gr_getch ();
			continue;
		}

		It = get_Itemc (pmons.pack, in);
		tried = true;
	}
	while (It == NULL);*/
	It = get_Itemc (th->pack, in);

	return It;
}

int mons_hits (struct Monster *from, struct Monster *to, struct Item *with)
{
	if ((to->status.defending.ydir || to->status.defending.xdir) &&
	    to->status.defending.ydir + to->yloc == from->yloc &&
	    to->status.defending.xdir + to->xloc == from->xloc)
		return 0;
	if (!with)
		return 1; // evading?
	return 1;
}

int mons_hitdmg (struct Monster *from, struct Monster *to, struct Item *with)
{
	if (!with)
		return 1 + rn(2);
	uint32_t attr = with->type.attr;
	int ret = rnd((attr>>4)&15, attr&15);
	return ret;
}

int mons_ST_hit (struct Monster *from, struct Item *with)
{
	if (!with)
		return 3;
	return 3 + (with->cur_weight)/500;
}

int mons_HP_regen (struct Monster *th)
{
	return !rn(50);
}

int mons_HP_max_regen (struct Monster *th)
{
	return 0;
}

int mons_ST_regen (struct Monster *th)
{
	return rn(th->con/2);
}

int mons_ST_max_regen (struct Monster *th)
{
	if (th->ST >= th->ST_max)
		return 0;
	return !rn(10*th->ST_max);
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

int AI_weapcmp (struct Monster *ai, struct Item *w1, struct Item *w2)
{
	if (NO_ITEM(w2))
		return !NO_ITEM(w1);
	if (NO_ITEM(w1))
		return -1;
	return ((uintptr_t)w1) > ((uintptr_t)w2);
}

int AI_TIMID_take_turn (struct Monster *ai)
{
	int y = rn(3)-1, x = rn(3)-1;
	int can = can_amove (get_sqattr (dlv_lvl(ai->dlevel), ai->yloc + y, ai->xloc + x));
	if (can == 1)
		mons_move (ai, y, x);
	else
		ev_queue (ai->speed, (union Event) { .mwait = {EV_MWAIT, ai->ID}});
	return 1;
}

int AI_HOSTILE_take_turn (struct Monster *ai)
{
	// TODO attack player on sight
	int y = rn(3)-1, x = rn(3)-1;
	int can = can_amove (get_sqattr (dlv_lvl(ai->dlevel), ai->yloc + y, ai->xloc + x));
	if (can == 1)
		mons_move (ai, y, x);
	else
		ev_queue (ai->speed, (union Event) { .mwait = {EV_MWAIT, ai->ID}});
	return 1;
}

int AI_AGGRO_take_turn (struct Monster *ai)
{
	TID aiID = ai->ID;
	struct Monster *to = MTHIID (ai->ctr.aggro.ID);
	if (!to)
	{
		ev_queue (0, (union Event) { .mturn = {EV_MTURN, aiID}});
		ev_queue (0, (union Event) { .mcalm = {EV_MCALM, aiID}});
		return 1;
	}

	if (!ai->pack)
		goto skip_weapon;
	/* if you have a better weapon then wield it */
	struct Item *curweap = ai->wearing.weaps[0];
	struct Item *bestweap = curweap;
	int i;
	/* find the best weapon in the monster's inventory, defaulting to the
	 * currently wielded weapon */
	for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
	{
		struct Item *item = ai->pack->items[i];
		if (!item)
			continue;
		if (AI_weapcmp (ai, item, bestweap) > 0)
			bestweap = item;
	}
	if (bestweap == curweap)
		goto skip_weapon;
	ev_queue (0, (union Event) { .mwield = {EV_MWIELD, ai->ID, 0, bestweap}});

skip_weapon: ;
	/* where you are */
	int aiy = ai->yloc, aix = ai->xloc;

	/* move randomly if you can't see your target */
	if (!bres_draw (aiy, aix, NULL, dlv_attr(ai->dlevel), NULL, to->yloc, to->xloc))
	{
		if (!mons_move (ai, rn(3) - 1, rn(3) - 1))
			ev_queue (ai->speed, (union Event) { .mwait = {EV_MWAIT, aiID}});
		return 1;
	}

	/* otherwise you can see your target; this is where you aim to travel */
	int toy = to->yloc + to->status.moving.ydir;
	int tox = to->xloc + to->status.moving.xdir;

	/* don't aim at yourself; TODO better movement prediction */
	if (toy == aiy && tox == aix)
	{
		toy = to->yloc;
		tox = to->xloc;
	}

	/* the direction you should go in */
	int ymove = (aiy < toy) - (aiy > toy);
	int xmove = (aix < tox) - (aix > tox);

	/* attack if that is where your target will be */
	if (aiy + ymove == toy && aix + xmove == tox)
	{
		ev_queue (0, (union Event) { .mattkm = {EV_MATTKM, aiID, ymove, xmove}});
		ev_queue (ai->speed+1, (union Event) { .mturn = {EV_MTURN, aiID}});
		return 1;
	}

	/* move towards your target if you can */
	if (can_amove (get_sqattr (dlv_lvl(ai->dlevel), ai->yloc + ymove, ai->xloc + xmove)))
	{
		ev_queue (0, (union Event) { .mmove = {EV_MMOVE, aiID, ymove, xmove}});
		ev_queue (ai->speed+1, (union Event) { .mturn = {EV_MTURN, aiID}});
		return 1;
	}

	/* otherwise try similar directions */
	if (can_amove (get_sqattr (dlv_lvl(ai->dlevel), ai->yloc, ai->xloc + xmove)))
	{
		ev_queue (0, (union Event) { .mmove = {EV_MMOVE, aiID, 0, xmove}});
		ev_queue (ai->speed+1, (union Event) { .mturn = {EV_MTURN, aiID}});
		return 1;
	}
	if (can_amove (get_sqattr (dlv_lvl(ai->dlevel), ai->yloc + ymove, ai->xloc)))
	{
		ev_queue (0, (union Event) { .mmove = {EV_MMOVE, aiID, ymove, 0}});
		ev_queue (ai->speed+1, (union Event) { .mturn = {EV_MTURN, aiID}});
		return 1;
	}
	ev_queue (ai->speed+1, (union Event) { .mturn = {EV_MTURN, aiID}});
	return 1;
}

