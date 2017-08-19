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
	TID ID;
	int can;
	struct Monster *self, *mons;
	Vector pickup, items;
	switch (ev->type)
	{
	  case EV_MMOVE:
		//printf("mmove\n");
		ID = ev->mmove.thID;
		th = THIID(ID);
		if (!th) return;
		// Next turn
		ev_queue (1, (union Event) { .mturn = {EV_MTURN, ID}});
		can = can_amove (get_sqattr (dlv_things(th->dlevel), ev->mmove.ydest, ev->mmove.xdest));
		if (can == 1)
			thing_move (th, th->dlevel, ev->mmove.ydest, ev->mmove.xdest);
		return;
	  case EV_MATTK:
		//printf("mattk\n");
		ID = ev->mmove.thID;
		fr = THIID(ID);
		if (!fr) return;
		// Next turn
		ev_queue (1, (union Event) { .mturn = {EV_MTURN, ID}});
		can = can_amove (get_sqattr (dlv_things(fr->dlevel), ev->mattk.ydest, ev->mattk.xdest));
		if (can != 2) return;
		to = get_sqmons(dlv_things(fr->dlevel), ev->mattk.ydest, ev->mattk.xdest);
		if (!to) return;
		to->thing.mons.HP -= 3;
		if (to->thing.mons.HP <= 0)
		{
			// Kill event
			ev_queue (0, (union Event) { .mkill = {EV_MKILL, ID, to->ID}});
		}
		return;
	  case EV_MKILL:
		//printf("mkill\n");
		fr = THIID(ev->mkill.frID); to = THIID(ev->mkill.toID);
		if (to == player)
		{
			p_msg ("You die...");
			gr_getch ();
			U.playing = PLAYER_LOSTGAME;
			return;
		}
		if (fr == player)
		{
			if (to->thing.mons.type == MTYP_SATAN)
				U.playing = PLAYER_WONGAME;
			pmons.exp += all_mons[to->thing.mons.type].exp;
		}
		rem_id (to->ID);
		return;
	  case EV_MTURN:
		//printf("mturn\n");
		th = THIID(ev->mturn.thID);
		if (!th) return;
		mons_take_move (th);
		return;
	  case EV_MGEN:
		mons_gen (cur_dlevel, 2, U.luck-30);
		// Next monster gen
		ev_queue (MGEN_DELAY, (union Event) { .mgen = {EV_MGEN}});
		return;
	  case EV_MREGEN:
		th = THIID(ev->mregen.thID);
		if (!th) return;
		// Next regen
		ev_queue (mons_tregen (th), (union Event) { .mregen = {EV_MREGEN, ev->mregen.thID}});

		self = &th->thing.mons;

		/* HP */
		if (rn(50) < U.attr[AB_CO])
			self->HP += (self->level + 10) / 10;
		if (self->HP > self->HP_max)
			self->HP = self->HP_max;
		self->HP_rec = ((10.0 + self->level)/10) * ((float)U.attr[AB_CO] / 50.0);

		/* ST */
		self->ST += rn(2);
		if (self->ST > self->ST_max)
			self->ST = self->ST_max;
		self->ST_rec = 0.5;
		return;
	  case EV_MPICKUP:
		/* Put items in ret_list into inventory. The loop
		* continues until ret_list is done or the pack is full. */
		pickup = ev->mpickup.things;
		TID ID = ev->mpickup.thID;
		th = THIID(ID);
		if (!th) return;
		mons = &(th->thing.mons);
		int i;
		for (i = 0; i < pickup->len; ++ i)
		{
			/* Pick up the item; quit if the bag is full */
			th = THIID(*(int*)v_at (pickup, i));
			if (!pack_add (&mons->pack, &th->thing.item))
				break;
			/* Remove item from main play */
			rem_id (th->ID);
		}
		v_free (pickup);
		// Next turn
		ev_queue (mons->speed, (union Event) { .mturn = {EV_MTURN, ID}});
		return;
	  case EV_MDROP:
		items = ev->mdrop.items;
		th = THIID (ev->mdrop.thID);
		if (!th) return;
		mons = &th->thing.mons;
		for (i = 0; i < items->len; ++ i)
		{
			struct Item **drop = v_at (items, i);
			if (*drop == mons->wearing.rweap)
				mons_unwield (th);
			unsigned u = PACK_AT (get_Itref (mons->pack, *drop));
			mons->pack.items[u] = NULL;
			new_thing (THING_ITEM, cur_dlevel, th->yloc, th->xloc, *drop);
		}
		v_free (items);
		// Next turn
		ev_queue (mons->speed, (union Event) { .mturn = {EV_MTURN, ev->mdrop.thID}});
		return;
	  case EV_NONE:
	  default:
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

