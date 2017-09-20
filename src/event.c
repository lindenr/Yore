/* event.c */

#include "include/thing.h"
#include "include/dlevel.h"
#include "include/monst.h"
#include "include/vector.h"
#include "include/panel.h"
#include "include/event.h"
#include "include/generate.h"
#include "include/rand.h"

void ev_print (struct QEv *qe);
Vector events = NULL;
Tick curtick = 0;
void ev_queue (int udelay, union Event ev)
{
	int when = curtick + udelay;
	//printf ("queue %d %d\n", when, ev.type);
	struct QEv qe = {when, ev};
	int i;
	for (i = 0; i < events->len; ++ i)
		ev_print (v_at (events, i));
	for (i = 0; i < events->len && ((struct QEv*)v_at(events, i))->tick < when; ++ i)
		{}
	v_push (events, &qe);
	memmove (v_at (events, i+1), v_at (events, i), events->siz * (events->len - i - 1));
	memcpy (v_at (events, i), &qe, events->siz);
}

void ev_print (struct QEv *qe)
{
	//printf ("list %d %d\n", qe->tick, qe->ev.type);
}

void ev_do (Event ev)
{
	struct Thing *th, *fr, *to;
	TID thID, frID, toID;
	int can, ydest, xdest;
	struct Monster *self, *mons;
	Vector pickup, items;
	switch (ev->type)
	{
	case EV_MRESET:
		thID = ev->mevade.thID;
		th = THIID(thID);
		if (!th)
			return;
		th->thing.mons.status.evading = 0;
		// Next turn
		ev_queue (0, (union Event) { .mturn = {EV_MTURN, thID}});
		return;
	case EV_MMOVE:
		thID = ev->mmove.thID;
		th = THIID(thID);
		if (!th)
			return;
		ev_queue (th->thing.mons.speed, (union Event) { .mdomove = {EV_MDOMOVE, thID}});
		th->thing.mons.status.moving.ydir = ev->mmove.ydir;
		th->thing.mons.status.moving.xdir = ev->mmove.xdir;
		return;
	case EV_MDOMOVE:
		//printf("mdomove\n");
		thID = ev->mdomove.thID;
		th = THIID(thID);
		if (!th)
			return;
		// Next turn
		ev_queue (0, (union Event) { .mturn = {EV_MTURN, thID}});
		ydest = th->yloc + th->thing.mons.status.moving.ydir; xdest = th->xloc + th->thing.mons.status.moving.xdir;
		can = can_amove (get_sqattr (dlv_things(th->dlevel), ydest, xdest));
		th->thing.mons.status.moving.ydir = 0;
		th->thing.mons.status.moving.xdir = 0;
		if (can == 1)
			thing_move (th, th->dlevel, ydest, xdest);
		return;
	case EV_MEVADE:
		thID = ev->mevade.thID;
		th = THIID(thID);
		if (!th)
			return;
		th->thing.mons.status.evading = 1;
		// Next turn
		ev_queue (th->thing.mons.speed/2, (union Event) { .mreset = {EV_MRESET, thID}});
		return;
	case EV_MATTKM:
		thID = ev->mattkm.thID;
		th = THIID(thID);
		if (!th)
			return;
		ev_queue (th->thing.mons.speed, (union Event) { .mdoattkm = {EV_MDOATTKM, thID}});
		th->thing.mons.status.attacking.ydir = ev->mattkm.ydir;
		th->thing.mons.status.attacking.xdir = ev->mattkm.xdir;
		break;
	case EV_MDOATTKM:
		frID = ev->mdoattkm.thID;
		fr = THIID(frID); /* get from-mons */
		if (!fr)
			return;
		ev_queue (0, (union Event) { .mturn = {EV_MTURN, frID}}); /* next turn of from-mons */
		ydest = fr->yloc + fr->thing.mons.status.attacking.ydir; xdest = fr->xloc + fr->thing.mons.status.attacking.xdir;
		fr->thing.mons.status.attacking.ydir = 0;
		fr->thing.mons.status.attacking.xdir = 0;
		can = can_amove (get_sqattr (dlv_things(fr->dlevel), ydest, xdest));
		if (can != 2)
			return;
		to = get_sqmons(dlv_things(fr->dlevel), ydest, xdest); /* get to-mons */
		if (!to)
			return;
		toID = to->ID;
		ev_queue (0, (union Event) { .mangerm = {EV_MANGERM, frID, toID}}); /* anger to-mons */
		int stamina_cost = mons_ST_hit (fr);
		if (fr->thing.mons.ST < stamina_cost)
		{
			p_msg ("The %s tiredly misses the %s!", fr->thing.mons.type->name, to->thing.mons.type->name); /* notify */
			return;
		}
		fr->thing.mons.ST -= stamina_cost;
		if (!mons_hits (fr, to))
		{
			p_msg ("The %s misses the %s!", fr->thing.mons.type->name, to->thing.mons.type->name); /* notify */
			return;
		}
		p_msg ("The %s hits the %s!", fr->thing.mons.type->name, to->thing.mons.type->name); /* notify */
		int damage = mons_hitdmg (fr, to);
		to->thing.mons.HP -= damage;
		if (to->thing.mons.HP <= 0)
		{
			to->thing.mons.HP = 0;
			ev_queue (0, (union Event) { .mkillm = {EV_MKILLM, frID, toID}}); /* kill to-mons */
		}
		return;
	case EV_MKILLM:
		fr = THIID(ev->mkillm.frID); to = THIID(ev->mkillm.toID);
		if ((!fr) || (!to))
			return;
		p_msg ("The %s kills the %s!", fr->thing.mons.type->name, to->thing.mons.type->name);
		if (mons_isplayer(to))
		{
			p_msg ("You die...");
			p_pane (to);
			gr_getch ();
			U.playing = PLAYER_LOSTGAME;
			return;
		}
		fr->thing.mons.exp += to->thing.mons.type->exp;
		ev_queue (0, (union Event) { .mcorpse = {EV_MCORPSE, ev->mkillm.toID}});
		return;
	case EV_MCORPSE:
		th = THIID(ev->mcorpse.thID);
		if (!th)
			return;

		/* add corpse */
		struct Item corpse;
		mons_corpse (th, &corpse.type);
		corpse.attr = 0;
		corpse.name = NULL;
		corpse.cur_weight = 0;
		new_thing (THING_ITEM, dlv_lvl (th->dlevel), th->yloc, th->xloc, &corpse);

		/* remove dead monster */
		rem_id (th->ID);
		return;
	case EV_MTURN:
		th = THIID(ev->mturn.thID);
		if (!th)
			return;
		mons_take_turn (th);
		return;
	case EV_MGEN:
		mons_gen (cur_dlevel, 2, U.luck-30);
		ev_queue (mons_tmgen (), (union Event) { .mgen = {EV_MGEN}}); /* Next monster gen */
		return;
	case EV_MREGEN:
		th = THIID(ev->mregen.thID);
		if (!th)
			return;
		ev_queue (mons_tregen (th), (union Event) { .mregen = {EV_MREGEN, ev->mregen.thID}}); /* Next regen */
		self = &th->thing.mons;

		/* HP */
		int HP_regen = mons_HP_regen (th), HP_max_regen = mons_HP_max_regen (th);
		self->HP += HP_regen;
		self->HP_max += HP_max_regen;
		self->HP_rec = 0.0;

		/* ST */
		int ST_regen = mons_ST_regen (th), ST_max_regen = mons_ST_max_regen (th);
		self->ST += ST_regen;
		self->ST_max += ST_max_regen;
		self->ST_rec = 0.0;
		return;
	case EV_MPICKUP:
		/* Put items in ret_list into inventory. The loop
		 * continues until ret_list is done or the pack is full. */
		pickup = ev->mpickup.things;
		thID = ev->mpickup.thID;
		th = THIID(thID);
		if (!th)
			return;
		mons = &(th->thing.mons);
		int i;
		for (i = 0; i < pickup->len; ++ i)
		{
			/* Pick up the item; quit if the bag is full */
			struct Thing *item = THIID(*(int*)v_at (pickup, i));
			if (pack_add (&mons->pack, &item->thing.item))
			{
				/* Say so */
				char *msg = get_inv_line (mons->pack, &item->thing.item);
				p_msg ("%s", msg);
				free (msg);
			}
			else
			{
				p_msg ("No more space. :/");
				break;
			}
			/* Remove item from main play */
			rem_id (item->ID);
		}
		v_free (pickup);
		// Next turn
		ev_queue (mons->speed, (union Event) { .mturn = {EV_MTURN, thID}});
		return;
	case EV_MDROP:
		th = THIID (ev->mdrop.thID);
		if (!th)
			return;
		items = ev->mdrop.items;
		mons = &th->thing.mons;
		for (i = 0; i < items->len; ++ i)
		{
			struct Item **drop = v_at (items, i);
			//if (*drop == mons->wearing.rweap)
			//	mons_unwield (th);
			unsigned u = PACK_AT (get_Itref (mons->pack, *drop));
			pack_rem (mons->pack, u);
			new_thing (THING_ITEM, cur_dlevel, th->yloc, th->xloc, *drop);
		}
		v_free (items);
		// Next turn
		ev_queue (mons->speed, (union Event) { .mturn = {EV_MTURN, ev->mdrop.thID}});
		return;
	case EV_MANGERM:
		frID = ev->mangerm.frID; toID = ev->mangerm.toID;
		fr = THIID(frID); to = THIID(toID);
		if ((!fr) || (!to))
			return;
		if (to->thing.mons.ai.mode == AI_NONE)
			return;
		if (to->thing.mons.ai.mode != AI_AGGRO)
			p_msg ("The %s angers the %s!", fr->thing.mons.type->name, to->thing.mons.type->name);
		to->thing.mons.ai.mode = AI_AGGRO;
		to->thing.mons.ai.aggro.ID = frID;
		return;
	case EV_MCALM:
		thID = ev->mcalm.thID;
		th = THIID(thID);
		if (!th)
			return;
		p_msg ("The %s calms.", th->thing.mons.type->name);
		th->thing.mons.ai.mode = AI_TIMID;
		return;
	case EV_MCHARGE:
		thID = ev->mcharge.thID;
		th = THIID(thID);
		if (!th)
			return;
		ev_queue (th->thing.mons.speed, (union Event) { .mturn = {EV_MTURN, thID}});
		return;
	case EV_MOPENDOOR:
		thID = ev->mopendoor.thID;
		th = THIID(thID);
		if (!th)
			return;
		ev_queue (th->thing.mons.speed, (union Event) { .mturn = {EV_MTURN, thID}});
		return;
	case EV_MCLOSEDOOR:
		thID = ev->mclosedoor.thID;
		th = THIID(thID);
		if (!th)
			return;
		ev_queue (th->thing.mons.speed, (union Event) { .mturn = {EV_MTURN, thID}});
		return;
	case EV_NONE:
		return;
	}
}

void ev_init ()
{
	events = v_dinit (sizeof(struct QEv));
}

void ev_loop ()
{
	while (U.playing == PLAYER_PLAYING)
	{
		////printf ("CURTICK=%d\n", curtick);
		struct QEv qe;
		memcpy (&qe, v_at (events, 0), sizeof (qe));
		//ev_print (&qe);
		v_rem (events, 0);
		
		curtick = qe.tick;

		ev_do (&qe.ev);
	}
}

