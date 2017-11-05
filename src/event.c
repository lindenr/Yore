/* event.c */

#include "include/thing.h"
#include "include/dlevel.h"
#include "include/monst.h"
#include "include/vector.h"
#include "include/panel.h"
#include "include/event.h"
#include "include/generate.h"
#include "include/rand.h"
#include "include/player.h"

Tick ev_delay (union Event *event)
{
	switch (event->type)
	{
	case EV_NONE:
		break;
	case EV_MWAIT:
		return (MTHIID(event->mwait.thID))->speed;
	case EV_MMOVE:
		return 0;
	case EV_MDOMOVE:
		return (MTHIID(event->mdomove.thID))->speed;
	case EV_MEVADE:
		return 0;
	case EV_MUNEVADE:
		return (MTHIID(event->munevade.thID))->speed/2;
	case EV_MSHIELD:
		return 0;
	case EV_MDOSHIELD:
		return 0;
	case EV_MUNSHIELD:
		return 0;
	case EV_MATTKM:
		return 0;
	case EV_MDOATTKM:
		return (MTHIID(event->mdoattkm.thID))->speed;
	case EV_MKILLM:
		return 0;
	case EV_MCORPSE:
		return 0;
	case EV_MTURN:
		return 0;
	case EV_MGEN:
		return mons_tmgen ();
	case EV_MREGEN:
		return mons_tregen (MTHIID(event->mregen.thID));
	case EV_MWIELD:
		// TODO: should be 0 if just unwielded something
		return (MTHIID(event->mwield.thID))->speed;
	case EV_MPICKUP:
		return (MTHIID(event->mpickup.thID))->speed;
	case EV_MDROP:
		// TODO: should be 0 if just unwielded the dropped thing
		return (MTHIID(event->mdrop.thID))->speed;
	case EV_MANGERM:
		return 0;
	case EV_MCALM:
		return 0;
	case EV_MCHARGE:
		return 0;
	case EV_MOPENDOOR:
		return 0;
	case EV_MCLOSEDOOR:
		return 0;
	}
	panic ("ev_delay reached end");
	return 0;
}

void ev_print (struct QEv *qe);
Vector events = NULL;
Tick curtick = 0;
void ev_queue (Tick udelay, union Event ev)
{
	Tick when = curtick + udelay; //ev_delay (&ev);
	//printf ("queue %d %d\n", when, ev.type);
	struct QEv qe = {when, ev};
	int i = 0;
	//for (i = 0; i < events->len; ++ i)
	//	ev_print (v_at (events, i));
	if (udelay)
	for (i = 0; i < events->len && ((struct QEv*)v_at(events, i))->tick <= when; ++ i)
		{}
	v_push (events, &qe);
	memmove (v_at (events, i+1), v_at (events, i), events->siz * (events->len - i - 1));
	memcpy (v_at (events, i), &qe, events->siz);
}

void ev_debug ()
{
	int i;
	printf ("curtick is %llu\n", curtick);
	for (i = 0; i < events->len; ++ i)
	{
		struct QEv *qe = v_at (events, i);
		printf ("due at %llu: %d\n", qe->tick, qe->ev.type);
		if (qe->ev.type == EV_MTURN)
			printf ("     id %d\n", qe->ev.mturn.thID);
	}
	printf("\n");
}

void ev_print (struct QEv *qe)
{
	//printf ("list %d %d\n", qe->tick, qe->ev.type);
}

void ev_do (Event ev)
{
	struct Monster *th, *fr, *to;
	struct Thing *item;
	TID thID, frID, toID;
	int can, ydest, xdest;
	Vector pickup, items;
	switch (ev->type)
	{
	case EV_MWAIT:
		thID = ev->mwait.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		// Next turn
		ev_queue (0, (union Event) { .mturn = {EV_MTURN, thID}});
		return;
	case EV_MMOVE:
		thID = ev->mmove.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		ev_queue (th->speed, (union Event) { .mdomove = {EV_MDOMOVE, thID}});
		th->status.moving.ydir = ev->mmove.ydir;
		th->status.moving.xdir = ev->mmove.xdir;
		return;
	case EV_MDOMOVE:
		//printf("mdomove\n");
		thID = ev->mdomove.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		// Next turn
		//ev_queue (0, (union Event) { .mturn = {EV_MTURN, thID}});
		ydest = th->yloc + th->status.moving.ydir; xdest = th->xloc + th->status.moving.xdir;
		can = can_amove (get_sqattr (dlv_lvl(th->dlevel), ydest, xdest));
		th->status.moving.ydir = 0;
		th->status.moving.xdir = 0;
		if (can == 1)
			monsthing_move (th, th->dlevel, ydest, xdest);
		//else: tried and failed to move TODO
		return;
	case EV_MEVADE:
		thID = ev->mevade.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		th->status.evading = 1;
		// Next turn
		ev_queue (th->speed/2, (union Event) { .munevade = {EV_MUNEVADE, thID}});
		return;
	case EV_MUNEVADE:
		thID = ev->mevade.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		th->status.evading = 0;
		// Next turn
		//ev_queue (0, (union Event) { .mturn = {EV_MTURN, thID}});
		return;
	case EV_MSHIELD:
		thID = ev->mshield.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		ev_queue (th->speed, (union Event) { .mdoshield = {EV_MDOSHIELD, thID, ev->mshield.ydir, ev->mshield.xdir}});
		return;
	case EV_MDOSHIELD:
		thID = ev->mdoshield.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		ev_queue (th->speed, (union Event) { .munshield = {EV_MUNSHIELD, thID}});
		th->status.defending.ydir = ev->mdoshield.ydir;
		th->status.defending.xdir = ev->mdoshield.xdir;
		return;
	case EV_MUNSHIELD:
		thID = ev->munshield.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		th->status.defending.ydir = 0;
		th->status.defending.xdir = 0;
		return;
	case EV_MATTKM:
		thID = ev->mattkm.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		ev_queue (th->speed, (union Event) { .mdoattkm = {EV_MDOATTKM, thID}});
		th->status.attacking.ydir = ev->mattkm.ydir;
		th->status.attacking.xdir = ev->mattkm.xdir;
		break;
	case EV_MDOATTKM:
		frID = ev->mdoattkm.thID;
		fr = MTHIID(frID); /* get from-mons */
		if (!fr)
			return;
		//ev_queue (0, (union Event) { .mturn = {EV_MTURN, frID}}); /* next turn of from-mons */
		ydest = fr->yloc + fr->status.attacking.ydir; xdest = fr->xloc + fr->status.attacking.xdir;
		fr->status.attacking.ydir = 0;
		fr->status.attacking.xdir = 0;
		can = can_amove (get_sqattr (dlv_lvl(fr->dlevel), ydest, xdest));
		if (can != 2)
			return; // tried and failed to attack TODO
		to = &dlv_lvl(fr->dlevel)->mons[map_buffer(ydest, xdest)]; /* get to-mons */
		if (!to->ID)
			return;
		toID = to->ID;
		ev_queue (0, (union Event) { .mangerm = {EV_MANGERM, frID, toID}}); /* anger to-mons */
		struct Item *with = fr->wearing.weaps[0];
		int stamina_cost = mons_ST_hit (fr, with);
		if (fr->ST < stamina_cost)
		{
			p_msg ("The %s tiredly misses the %s!", fr->mname, to->mname); /* notify */
			return;
		}
		fr->ST -= stamina_cost;
		if (!mons_hits (fr, to, with))
		{
			p_msg ("The %s misses the %s!", fr->mname, to->mname); /* notify */
			return;
		}
		p_msg ("The %s hits the %s!", fr->mname, to->mname); /* notify */
		int damage = mons_hitdmg (fr, to, with);
		to->HP -= damage;
		if (to->HP <= 0)
		{
			to->HP = 0;
			ev_queue (0, (union Event) { .mkillm = {EV_MKILLM, frID, toID}}); /* kill to-mons */
		}
		return;
	case EV_MKILLM:
		fr = MTHIID(ev->mkillm.frID); to = MTHIID(ev->mkillm.toID);
		if ((!fr) || (!to))
			return;
		p_msg ("The %s kills the %s!", fr->mname, to->mname);
		if (mons_isplayer(to))
		{
			p_msg ("You die...");
			p_pane (to);
			gr_getch ();
			U.playing = PLAYER_LOSTGAME;
			return;
		}
		fr->exp += to->exp;
		ev_queue (0, (union Event) { .mcorpse = {EV_MCORPSE, ev->mkillm.toID}});
		return;
	case EV_MCORPSE:
		th = MTHIID(ev->mcorpse.thID);
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
		rem_mid (th->ID);
		return;
	case EV_MTURN:
		th = MTHIID(ev->mturn.thID);
		if (!th)
			return;
		mons_take_turn (th);
		return;
	case EV_MGEN:
		mons_gen (cur_dlevel, 2, U.luck-30);
		ev_queue (mons_tmgen (), (union Event) { .mgen = {EV_MGEN}}); /* Next monster gen */
		return;
	case EV_MREGEN:
		th = MTHIID(ev->mregen.thID);
		if (!th)
			return;
		ev_queue (mons_tregen (th), (union Event) { .mregen = {EV_MREGEN, ev->mregen.thID}}); /* Next regen */
		/* HP */
		int HP_regen = mons_HP_regen (th), HP_max_regen = mons_HP_max_regen (th);
		th->HP += HP_regen;
		th->HP_max += HP_max_regen;

		/* ST */
		int ST_regen = mons_ST_regen (th), ST_max_regen = mons_ST_max_regen (th);
		th->ST += ST_regen;
		th->ST_max += ST_max_regen;
		return;
	case EV_MWIELD:
		th = MTHIID(ev->mwield.thID);
		if (!th)
			return;
		int arm = ev->mwield.arm;
		struct Item *it = ev->mwield.it;
		if (th->wearing.weaps[arm])
			th->wearing.weaps[arm]->attr &= ~ITEM_WIELDED;
		if (it->type.type == ITYP_NONE)
		{
			th->wearing.weaps[arm] = NULL;
			//ev_queue (0, (union Event) { .mturn = {EV_MTURN, th->ID}});
			return;
		}
		th->wearing.weaps[arm] = it;
		it->attr |= ITEM_WIELDED;
		if (mons_isplayer(th))
			item_look (it);
		//ev_queue (0, (union Event) { .mturn = {EV_MTURN, th->ID}});
		return;
	case EV_MPICKUP:
		/* Put items in ret_list into inventory. The loop
		 * continues until ret_list is done or the pack is full. */
		pickup = ev->mpickup.things;
		thID = ev->mpickup.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		int i;
		for (i = 0; i < pickup->len; ++ i)
		{
			/* Pick up the item; quit if the bag is full */
			item = THIID(*(int*)v_at (pickup, i));
			if (pack_add (&th->pack, &item->thing.item))
			{
				/* Say so */
				char *msg = get_inv_line (th->pack, &item->thing.item);
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
		//ev_queue (0, (union Event) { .mturn = {EV_MTURN, thID}});
		return;
	case EV_MDROP:
		th = MTHIID (ev->mdrop.thID);
		if (!th)
			return;
		items = ev->mdrop.items;
		for (i = 0; i < items->len; ++ i)
		{
			struct Item **drop = v_at (items, i);
			if ((*drop)->attr & ITEM_WIELDED)
				continue;
			unsigned u = PACK_AT (get_Itref (th->pack, *drop));
			pack_rem (th->pack, u);
			new_thing (THING_ITEM, cur_dlevel, th->yloc, th->xloc, *drop);
			free(*drop);
		}
		v_free (items);
		// Next turn
		//ev_queue (0, (union Event) { .mturn = {EV_MTURN, ev->mdrop.thID}});
		return;
	case EV_MANGERM:
		frID = ev->mangerm.frID; toID = ev->mangerm.toID;
		fr = MTHIID(frID); to = MTHIID(toID);
		if ((!fr) || (!to))
			return;
		if (mons_isplayer (to))
			return;
		if (to->ai.mode != AI_AGGRO)
			p_msg ("The %s angers the %s!", fr->mname, to->mname);
		to->ai.mode = AI_AGGRO;
		to->ai.aggro.ID = frID;
		return;
	case EV_MCALM:
		thID = ev->mcalm.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		p_msg ("The %s calms.", th->mname);
		th->ai.mode = AI_TIMID;
		return;
	case EV_MCHARGE:
		thID = ev->mcharge.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		ev_queue (th->speed, (union Event) { .mwait = {EV_MWAIT, thID}});
		return;
	case EV_MOPENDOOR:
		thID = ev->mopendoor.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		ev_queue (th->speed, (union Event) { .mwait = {EV_MWAIT, thID}});
		return;
	case EV_MCLOSEDOOR:
		thID = ev->mclosedoor.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		ev_queue (th->speed, (union Event) { .mwait = {EV_MWAIT, thID}});
		return;
	case EV_NONE:
		return;
	}
}

void ev_init ()
{
	events = v_dinit (sizeof(struct QEv));
	player_actions = v_dinit (sizeof(struct QEv));
}

// TODO make events a heap
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
