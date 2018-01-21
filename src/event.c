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
#include "include/skills.h"

Tick ev_delay (union Event *event)
{
	switch (event->type)
	{
	case EV_NONE:
		break;
	case EV_WORLD_INIT:
	case EV_PLAYER_INIT:
	case EV_WORLD_HEARTBEAT:
		return 0;
	case EV_MTHROW:
		return (MTHIID(event->mdrop.thID))->speed;
	case EV_PROJ_MOVE:
	case EV_PROJ_DONE:
	case EV_PROJ_HIT_BARRIER:
	case EV_PROJ_HIT_MONSTER:
		return 0;
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
		return 0;
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
	case EV_MSTARTCHARGE:
		return 0;
	case EV_MDOCHARGE:
		return 0;
	case EV_MSTOPCHARGE:
		return 0;
	case EV_MFIREBALL:
	case EV_MWATER_BOLT:
	case EV_CIRCLEOFFLAME:
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
	Tick when = curtick + udelay;
	struct QEv qe = {when, ev};
	int i = 0;
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

void ev_mons_start (struct Monster *th)
{
	ev_queue (1000 + rn(1000), (union Event) { .mturn = {EV_MTURN, th->ID}});
	ev_queue (1, (union Event) { .mregen = {EV_MREGEN, th->ID}});
}

void ev_do (Event ev)
{
	struct Monster *th, *fr, *to;
	struct Item newitem, *item;
	TID thID, frID, toID, itemID;
	int can, ydest, xdest;
	int i, j;
	Vector pickup, items;
	union ItemLoc loc;
	switch (ev->type)
	{
	case EV_WORLD_INIT:
		ev_queue (0, (union Event) { .world_heartbeat = {EV_WORLD_HEARTBEAT}});
		ev_queue (0, (union Event) { .player_init = {EV_PLAYER_INIT}});
		generate_map (dlv_lvl (1), LEVEL_NORMAL);
		return;
	case EV_PLAYER_INIT:
		th = gen_player (50, 150, player_name);
		ev_mons_start (th);

#ifdef TWOPLAYER
		th = gen_player (52, 153, "Player 2");
		ev_mons_start (th);
#endif
		
		th = gen_boss (57, 160);
		ev_mons_start (th);
		return;
	case EV_WORLD_HEARTBEAT:
		/* monster generation */
		if (!rn(10))
			ev_queue (0, (union Event) { .mgen = {EV_MGEN}});
		/* next heartbeat */
		ev_queue (1000, (union Event) { .world_heartbeat = {EV_WORLD_HEARTBEAT}});
		return;
	case EV_MTHROW:
		thID = ev->mthrow.thID;
		th = MTHIID (thID);
		if (!th)
			return;
		itemID = ev->mthrow.itemID;
		item = ITEMID (itemID);
		if (!item)
			return;
		loc = (union ItemLoc) { .fl = {LOC_FLIGHT, th->dlevel, th->yloc, th->xloc, {0,}, th->str}};
		bres_init (&loc.fl.bres, th->yloc, th->xloc, ev->mthrow.ydest, ev->mthrow.xdest);
		item_put (item, loc);
		ev_queue (60, (union Event) { .proj_move = {EV_PROJ_MOVE, itemID}});
		return;
	case EV_PROJ_MOVE:
		itemID = ev->proj_move.itemID;
		item = ITEMID(itemID);
		if ((!item) || item->loc.loc != LOC_FLIGHT)
			return;
		if (item->loc.fl.speed <= 0)
		{
			ev_queue (0, (union Event) { .proj_done = {EV_PROJ_DONE, itemID}});
			return;
		}
		memcpy (&loc, &item->loc, sizeof(loc));
		bres_iter (&loc.fl.bres);
		if (!get_sqattr (dlv_lvl(loc.fl.dlevel), loc.fl.bres.cy, loc.fl.bres.cx))
		{
			ev_queue (0, (union Event) { .proj_hit_barrier = {EV_PROJ_HIT_BARRIER, itemID}});
			return;
		}
		loc.fl.yloc = loc.fl.bres.cy;
		loc.fl.xloc = loc.fl.bres.cx;
		loc.fl.speed -= 1;
		item_put (item, loc);
		ev_queue (60, (union Event) { .proj_move = {EV_PROJ_MOVE, itemID}});
		return;
	case EV_PROJ_DONE:
		itemID = ev->proj_done.itemID;
		item = ITEMID(itemID);
		if (item->type.type == ITYP_ARCANE)
		{
			p_msg ("The %s dissipates.", item->type.name);
			item_free (item);
			return;
		}
		p_msg ("The %s falls to the ground.", item->type.name);
		item_put (item, (union ItemLoc) { .dlvl =
			{LOC_DLVL, item->loc.fl.dlevel, item->loc.fl.yloc, item->loc.fl.xloc}});
		return;
	case EV_PROJ_HIT_BARRIER:
		itemID = ev->proj_hit_barrier.itemID;
		item = ITEMID(itemID);
		if (item->type.type == ITYP_ARCANE)
		{
			p_msg ("The %s is absorbed.", item->type.name);
			item_free (item);
			return;
		}
		p_msg ("The %s hits the wall.", item->type.name);
		item_put (item, (union ItemLoc) { .dlvl =
			{LOC_DLVL, item->loc.fl.dlevel, item->loc.fl.yloc, item->loc.fl.xloc}});
		return;
	case EV_PROJ_HIT_MONSTER:
		return;
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
		thID = ev->mdomove.thID;
		th = MTHIID(thID);
		if (!th)
			return;
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
		ev_queue (th->speed/2, (union Event) { .munevade = {EV_MUNEVADE, thID}});
		return;
	case EV_MUNEVADE:
		thID = ev->mevade.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		th->status.evading = 0;
		return;
	case EV_MSHIELD:
		thID = ev->mshield.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		ev_queue (th->speed/2 + 1, (union Event) { .mdoshield =
			{EV_MDOSHIELD, thID, ev->mshield.ydir, ev->mshield.xdir}});
		return;
	case EV_MDOSHIELD:
		thID = ev->mdoshield.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		ev_queue (th->speed/2 + 1, (union Event) { .munshield = {EV_MUNSHIELD, thID}});
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
		p_msg ("The %s swings, to hit in %dms!", th->mname, th->speed);
		break;
	case EV_MDOATTKM:
		frID = ev->mdoattkm.thID;
		fr = MTHIID(frID); /* get from-mons */
		if (!fr)
			return;
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
		int damage = mons_hitdmg (fr, to, with);
		p_msg ("The %s hits the %s for %d!", fr->mname, to->mname, damage); /* notify */
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

		if (!th->pack)
			goto mcorpse;
		for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
		{
			if (!th->pack->items[i])
				continue;
			item_put (th->pack->items[i], (union ItemLoc) { .dlvl =
				{LOC_DLVL, th->dlevel, th->yloc, th->xloc}});
			free (th->pack->items[i]);
		}
		free(th->pack);

	mcorpse: ;
		/* add corpse */
		struct Item corpse;
		mons_corpse (th, &corpse.type);
		corpse.ID = 0;
		corpse.loc.loc = LOC_NONE;
		corpse.attr = 0;
		corpse.name = NULL;
		corpse.cur_weight = 0;
		item_put (&corpse, (union ItemLoc) { .dlvl =
			{LOC_DLVL, th->dlevel, th->yloc, th->xloc}});

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
		th = gen_mons_in_level ();
		if (!th)
			return;
		ev_mons_start (th);
		return;
	case EV_MREGEN:
		th = MTHIID(ev->mregen.thID);
		if (!th)
			return;
		ev_queue (mons_tregen (th), (union Event) { .mregen =
			{EV_MREGEN, ev->mregen.thID}}); /* Next regen */
		/* HP */
		int HP_regen = mons_HP_regen (th), HP_max_regen = mons_HP_max_regen (th);
		th->HP += HP_regen;
		th->HP_max += HP_max_regen;
		if (th->HP > th->HP_max)
			th->HP = th->HP_max;

		/* ST */
		int ST_regen = mons_ST_regen (th), ST_max_regen = mons_ST_max_regen (th);
		th->ST += ST_regen;
		th->ST_max += ST_max_regen;
		if (th->ST > th->ST_max)
			th->ST = th->ST_max;
		return;
	case EV_MWIELD:
		th = MTHIID(ev->mwield.thID);
		if (!th)
			return;
		int arm = ev->mwield.arm;
		struct Item *it = ev->mwield.it;
		if (th->wearing.weaps[arm])
			mons_unwield (th, th->wearing.weaps[arm]);
		if (NO_ITEM(it))
			return;
		mons_wield (th, arm, it);
		th->wearing.weaps[arm] = it;
		char *msg = get_inv_line (it);
		p_msg ("The %s wields %s.", th->mname, msg); /* notify */
		free (msg);
		return;
	case EV_MPICKUP:
		/* Put items in ret_list into inventory. The loop
		 * continues until ret_list is done or the pack is full. */
		pickup = ev->mpickup.things;
		thID = ev->mpickup.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		if (!th->pack)
			th->pack = pack_init ();
		for (i = 0, j = 0; j < MAX_ITEMS_IN_PACK && i < pickup->len; ++ j)
		{
			if (th->pack->items[j])
				continue;
			itemID = *(int*)v_at (pickup, i);
			/* Pick up the item */
			item = ITEMID(itemID);
			item_put (item, (union ItemLoc) { .inv = {LOC_INV, thID, j}});
			/* Say so */
			char *msg = get_inv_line (ITEMID(itemID));
			p_msg ("%s", msg);
			free (msg);
			/* Next item */
			++ i;
		}
		if (i < pickup->len)
		{
			p_msg ("No more space. :/");
			break;
		}
		v_free (pickup);
		return;
	case EV_MDROP:
		th = MTHIID (ev->mdrop.thID);
		if (!th)
			return;
		items = ev->mdrop.items;
		for (i = 0; i < items->len; ++ i)
		{
			struct Item *drop = ITEMID(*(TID*)v_at (items, i));
			item_put (drop, (union ItemLoc) { .dlvl = {LOC_DLVL, th->dlevel, th->yloc, th->xloc}});
		}
		v_free (items);
		return;
	case EV_MANGERM:
		frID = ev->mangerm.frID; toID = ev->mangerm.toID;
		fr = MTHIID(frID); to = MTHIID(toID);
		if ((!fr) || (!to))
			return;
		if (mons_isplayer (to))
			return;
		if (to->ctr.mode != CTR_AI_AGGRO)
			p_msg ("The %s angers the %s!", fr->mname, to->mname);
		to->ctr.mode = CTR_AI_AGGRO;
		to->ctr.aggro.ID = frID;
		return;
	case EV_MCALM:
		thID = ev->mcalm.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		p_msg ("The %s calms.", th->mname);
		th->ctr.mode = CTR_AI_TIMID;
		return;
	case EV_MSTARTCHARGE:
		thID = ev->mstartcharge.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		th->status.charging = 1;
		return;
	case EV_MDOCHARGE:
		thID = ev->mdocharge.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		return;
	case EV_MSTOPCHARGE:
		thID = ev->mstopcharge.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		th->status.charging = 0;
		return;
	case EV_MFIREBALL:
		thID = ev->mfireball.thID;
		th = MTHIID (thID);
		if (!th)
			return;
		newitem = new_item (ityp_fireball);
		loc = (union ItemLoc) { .fl = {LOC_FLIGHT, th->dlevel, th->yloc, th->xloc, {0,}, th->str}};
		bres_init (&loc.fl.bres, th->yloc, th->xloc, ev->mfireball.ydest, ev->mfireball.xdest);
		item = item_put (&newitem, loc);
		ev_queue (60, (union Event) { .proj_move = {EV_PROJ_MOVE, item->ID}});
		return;
	case EV_MWATER_BOLT:
		thID = ev->mwater_bolt.thID;
		th = MTHIID (thID);
		if (!th)
			return;
		newitem = new_item (ityp_water_bolt);
		loc = (union ItemLoc) { .fl = {LOC_FLIGHT, th->dlevel, th->yloc, th->xloc, {0,}, th->str}};
		bres_init (&loc.fl.bres, th->yloc, th->xloc, ev->mwater_bolt.ydest, ev->mwater_bolt.xdest);
		item = item_put (&newitem, loc);
		ev_queue (60, (union Event) { .proj_move = {EV_PROJ_MOVE, item->ID}});
		return;
	case EV_CIRCLEOFFLAME:
		thID = ev->circleofflame.thID;
		th = MTHIID(thID);
		if (!th)
			return;
		sk_flames_overlay (th);
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
	ev_queue (0, (union Event) { .world_init = {EV_WORLD_INIT}});
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

