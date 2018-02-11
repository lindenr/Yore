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
		return (MTHIID(event->mdrop.thID))->speed /2;
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
	case EV_MLEVEL:
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
	case EV_MWEAR_ARMOUR:
	case EV_MTAKEOFF_ARMOUR:
		return 0;
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
	case EV_MFROST:
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

void ev_mons_start (struct Monster *mons)
{
	ev_queue (1000 + rn(1000), (union Event) { .mturn = {EV_MTURN, mons->ID}});
	ev_queue (1, (union Event) { .mregen = {EV_MREGEN, mons->ID}});
}

void ev_do (Event ev)
{
	struct Monster *mons, *fr, *to;
	struct Item newitem, *item;
	TID thID, frID, toID, itemID, monsID;
	int can, ydest, xdest, damage, radius;
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
		mons = gen_player (50, 150, player_name);
		ev_mons_start (mons);
		update_knowledge (mons);

#ifdef TWOPLAYER
		mons = gen_player (52, 153, "Player 2");
		ev_mons_start (mons);
		update_knowledge (mons);
#endif
		
		mons = gen_boss (57, 160);
		ev_mons_start (mons);

		/*for (i = 0; i < 100; ++ i)
		{
			mons = gen_mons_in_level ();
			if (!mons)
				continue;
			ev_mons_start (mons);
		}*/
		draw_map ();
		return;
	case EV_WORLD_HEARTBEAT:
		/* monster generation */
		if (!rn(3))
			ev_queue (0, (union Event) { .mgen = {EV_MGEN}});
		/* next heartbeat */
		ev_queue (1000, (union Event) { .world_heartbeat = {EV_WORLD_HEARTBEAT}});
		return;
	case EV_MTHROW:
		thID = ev->mthrow.thID;
		mons = MTHIID (thID);
		if (!mons)
			return;
		itemID = ev->mthrow.itemID;
		item = ITEMID (itemID);
		if (!item)
			return;
		int speed = mons_throwspeed (mons, item);
		if (speed <= 0)
		{
			p_msg ("The %s fails to throw the %s.", mons->mname, item->type.name);
			return;
		}
		loc = (union ItemLoc) { .fl =
			{LOC_FLIGHT, mons->dlevel, mons->yloc, mons->xloc, {0,}, speed, thID}};
		bres_init (&loc.fl.bres, mons->yloc, mons->xloc, ev->mthrow.ydest, ev->mthrow.xdest);
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
		thID = dlv_lvl(loc.fl.dlevel)->monsIDs[map_buffer(loc.fl.yloc, loc.fl.xloc)];
		if (thID)
			ev_queue (0, (union Event) { .proj_hit_monster = {EV_PROJ_HIT_MONSTER, itemID, thID}});
		ev_queue (60, (union Event) { .proj_move = {EV_PROJ_MOVE, itemID}});
		draw_map ();
		gr_refresh ();
		return;
	case EV_PROJ_DONE:
		itemID = ev->proj_done.itemID;
		item = ITEMID(itemID);
		if (item->type.type == ITSORT_ARCANE)
		{
			p_msg ("The %s dissipates.", item->type.name);
			item_free (item);
			return;
		}
		//p_msg ("The %s falls to the ground.", item->type.name); // is this message necessary?
		item_put (item, (union ItemLoc) { .dlvl =
			{LOC_DLVL, item->loc.fl.dlevel, item->loc.fl.yloc, item->loc.fl.xloc}});
		return;
	case EV_PROJ_HIT_BARRIER:
		itemID = ev->proj_hit_barrier.itemID;
		item = ITEMID(itemID);
		if (item->type.type == ITSORT_ARCANE)
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
		itemID = ev->proj_hit_monster.itemID;
		item = ITEMID(itemID);
		if ((!item) || item->loc.loc != LOC_FLIGHT)
			return;
		monsID = ev->proj_hit_monster.monsID;
		mons = MTHIID(monsID);
		if (!mons)
			return;
		ev_queue (0, (union Event) { .mangerm =
			{EV_MANGERM, item->loc.fl.frID, monsID}}); /* anger to-mons */
		if (!proj_hitm (item, mons))
		{
			p_msg ("The %s misses the %s!", item->type.name, mons->mname); /* notify */
			return;
		}
		damage = proj_hitdmg (item, mons);
		p_msg ("The %s hits the %s for %d!", item->type.name, mons->mname, damage); /* notify */
		mons->HP -= damage;
		if (mons->HP <= 0)
			ev_queue (0, (union Event) { .mkillm =
				{EV_MKILLM, item->loc.fl.frID, monsID}}); /* kill to-mons */
		item->loc.fl.speed = 0;
		fr = MTHIID (item->loc.fl.frID);
		if (!fr)
			return;
		if (mons_gets_exp (fr))
			mons_exercise (fr, item);
		return;
	case EV_MWAIT:
		thID = ev->mwait.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		// Next turn
		ev_queue (0, (union Event) { .mturn = {EV_MTURN, thID}});
		return;
	case EV_MMOVE:
		thID = ev->mmove.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		ev_queue (mons->speed, (union Event) { .mdomove = {EV_MDOMOVE, thID}});
		mons->status.moving.ydir = ev->mmove.ydir;
		mons->status.moving.xdir = ev->mmove.xdir;
		return;
	case EV_MDOMOVE:
		thID = ev->mdomove.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		ydest = mons->yloc + mons->status.moving.ydir; xdest = mons->xloc + mons->status.moving.xdir;
		can = can_amove (get_sqattr (dlv_lvl(mons->dlevel), ydest, xdest));
		mons->status.moving.ydir = 0;
		mons->status.moving.xdir = 0;
		if (can == 1)
			monsthing_move (mons, mons->dlevel, ydest, xdest);
		//else: tried and failed to move TODO
		if (mons_isplayer (mons))
		{
			/* check what the player can see now */
			update_knowledge (mons);
			/* re-eval paths to player */
			dlv_fill_player_dist (cur_dlevel);
		}
		return;
	case EV_MEVADE:
		thID = ev->mevade.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		mons->status.evading = 1;
		ev_queue (mons->speed/2, (union Event) { .munevade = {EV_MUNEVADE, thID}});
		return;
	case EV_MUNEVADE:
		thID = ev->mevade.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		mons->status.evading = 0;
		return;
	case EV_MSHIELD:
		thID = ev->mshield.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		ev_queue (mons->speed/2 + 1, (union Event) { .mdoshield =
			{EV_MDOSHIELD, thID, ev->mshield.ydir, ev->mshield.xdir}});
		return;
	case EV_MDOSHIELD:
		thID = ev->mdoshield.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		ev_queue (mons->speed/2 + 1, (union Event) { .munshield = {EV_MUNSHIELD, thID}});
		mons->status.defending.ydir = ev->mdoshield.ydir;
		mons->status.defending.xdir = ev->mdoshield.xdir;
		return;
	case EV_MUNSHIELD:
		thID = ev->munshield.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		mons->status.defending.ydir = 0;
		mons->status.defending.xdir = 0;
		return;
	case EV_MATTKM:
		thID = ev->mattkm.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		ev_queue (mons->speed, (union Event) { .mdoattkm = {EV_MDOATTKM, thID}});
		mons->status.attacking.ydir = ev->mattkm.ydir;
		mons->status.attacking.xdir = ev->mattkm.xdir;
		mons->status.attacking.arm = ev->mattkm.arm;
		//p_msg ("The %s swings, to hit in %dms!", mons->mname, mons->speed);
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
		toID = dlv_lvl(fr->dlevel)->monsIDs[map_buffer(ydest, xdest)]; /* get to-mons */
		if (!toID)
			return;
		to = MTHIID(toID);
		ev_queue (0, (union Event) { .mangerm = {EV_MANGERM, frID, toID}}); /* anger to-mons */
		struct Item *with = fr->wearing.weaps[fr->status.attacking.arm];
		fr->status.attacking.arm = -1;
		int stamina_cost = mons_ST_hit (fr, with);
		if (fr->ST < stamina_cost)
		{
			p_msg ("The %s tiredly misses the %s!", fr->mname, to->mname); /* notify */
			return;
		}
		fr->ST -= stamina_cost;
		if (!mons_hitm (fr, to, with))
		{
			p_msg ("The %s misses the %s!", fr->mname, to->mname); /* notify */
			return;
		}
		damage = mons_hitdmg (fr, to, with);
		if (damage == 0)
		{
			p_msg ("The %s just misses the %s!", fr->mname, to->mname); /* notify */
			return;
		}
		p_msg ("The %s hits the %s for %d!", fr->mname, to->mname, damage); /* notify */
		to->HP -= damage;
		if (to->HP <= 0)
		{
			to->HP = 0;
			ev_queue (0, (union Event) { .mkillm = {EV_MKILLM, frID, toID}}); /* kill to-mons */
		}
		if (mons_gets_exp (fr))
			mons_exercise (fr, with);
		return;
	case EV_MKILLM:
		fr = MTHIID(ev->mkillm.frID); to = MTHIID(ev->mkillm.toID);
		if ((!fr) || (!to))
			return;
		p_msg ("The %s kills the %s!", fr->mname, to->mname);
		if (mons_isplayer(to))
		{
			p_pane (to);
			p_msgbox ("You die...");
			U.playing = PLAYER_LOSTGAME;
			return;
		}
		if (mons_gets_exp (fr))
		{
			fr->exp += to->exp;
			if (mons_level (fr->exp) != fr->level)
				ev_queue (0, (union Event) { .mlevel = {EV_MLEVEL, fr->ID}});
		}
		ev_queue (0, (union Event) { .mcorpse = {EV_MCORPSE, ev->mkillm.toID}}); /* dead drop */
		return;
	case EV_MCORPSE:
		mons = MTHIID(ev->mcorpse.thID);
		if (!mons)
			return;

		if (!mons->pack)
			goto mcorpse;
		for (i = 0; i < MAX_ITEMS_IN_PACK; ++ i)
		{
			struct Item *packitem = &mons->pack->items[i];
			if (NO_ITEM(packitem))
				continue;
			item_put (packitem, (union ItemLoc) { .dlvl =
				{LOC_DLVL, mons->dlevel, mons->yloc, mons->xloc}});
		}
		free(mons->pack);

	mcorpse: ;
		/* item drops */
		if (!rn(5))
			item_gen ((union ItemLoc) { .dlvl =
				{LOC_DLVL, mons->dlevel, mons->yloc, mons->xloc}});
		/* add corpse */
		struct Item corpse;
		mons_corpse (mons, &corpse);
		item_put (&corpse, (union ItemLoc) { .dlvl =
			{LOC_DLVL, mons->dlevel, mons->yloc, mons->xloc}});

		/* remove dead monster */
		rem_mid (mons->ID);
		return;
	case EV_MLEVEL:
		mons = MTHIID(ev->mlevel.thID);
		if (!mons)
			return;
		mons_level_up (mons);
		return;
	case EV_MTURN:
		mons = MTHIID(ev->mturn.thID);
		if (!mons)
			return;
		if (mons_isplayer (mons))
		{
			draw_map ();
		}
		mons_take_turn (mons);
		return;
	case EV_MGEN:
		mons = gen_mons_in_level ();
		if (!mons)
			return;
		ev_mons_start (mons);
		return;
	case EV_MREGEN:
		mons = MTHIID(ev->mregen.thID);
		if (!mons)
			return;
		ev_queue (mons_tregen (mons), (union Event) { .mregen =
			{EV_MREGEN, ev->mregen.thID}}); /* Next regen */
		/* HP */
		int HP_regen = mons_HP_regen (mons);
		mons->HP += HP_regen;
		if (mons->HP > mons->HP_max)
			mons->HP = mons->HP_max;
		/* ST */
		int ST_regen = mons_ST_regen (mons);
		mons->ST += ST_regen;
		if (mons->ST > mons->ST_max)
			mons->ST = mons->ST_max;
		/* MP */
		int MP_regen = mons_MP_regen (mons);
		mons->MP += MP_regen;
		if (mons->MP > mons->MP_max)
			mons->MP = mons->MP_max;
		return;
	case EV_MWIELD:
		mons = MTHIID(ev->mwield.thID);
		if (!mons)
			return;
		int arm = ev->mwield.arm;
		if (mons->status.attacking.arm == arm)
			return;
		if (mons->wearing.weaps[arm])
			mons_unwield (mons, mons->wearing.weaps[arm]);
		struct Item *it = ITEMID(ev->mwield.itemID);
		if ((!it) || it->loc.loc != LOC_INV || it->loc.inv.monsID != ev->mwield.thID)
			return;
		char *msg = get_inv_line (it);
		p_msg ("The %s wields %s.", mons->mname, msg); /* notify */
		free (msg);
		mons_wield (mons, arm, it);
		mons->wearing.weaps[arm] = it;
		return;
	case EV_MWEAR_ARMOUR:
		mons = MTHIID(ev->mwear_armour.thID);
		if (!mons)
			return;
		item = ITEMID (ev->mwear_armour.itemID); 
		if ((!item) || item->loc.loc != LOC_INV ||
			item->loc.inv.monsID != ev->mwear_armour.thID ||
			item_worn(item))
			return;
		if (!mons_can_wear (mons, item, ev->mwear_armour.offset))
			return;
		msg = get_inv_line (item);
		p_msg ("The %s wears %s.", mons->mname, msg); /* notify */
		free (msg);
		*(struct Item **)((char*)&mons->wearing + ev->mwear_armour.offset) = item;
		item->worn_offset = ev->mwear_armour.offset;
		mons->armour += item->def;
		return;
	case EV_MTAKEOFF_ARMOUR:
		mons = MTHIID(ev->mtakeoff_armour.thID);
		if (!mons)
			return;
		item = ITEMID (ev->mtakeoff_armour.itemID); 
		if ((!item) || item->loc.loc != LOC_INV ||
			item->loc.inv.monsID != ev->mtakeoff_armour.thID ||
			(!item_worn(item)))
			return;
		if (!mons_can_takeoff (mons, item))
			return;
		*(struct Item **)((char*)&mons->wearing + item->worn_offset) = 0;
		item->worn_offset = -1;
		msg = get_inv_line (item);
		p_msg ("The %s takes off %s.", mons->mname, msg); /* notify */
		free (msg);
		mons->armour -= item->def;
		return;
	case EV_MPICKUP:
		/* Put items in ret_list into inventory. The loop
		 * continues until ret_list is done or the pack is full. */
		pickup = ev->mpickup.things;
		thID = ev->mpickup.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		if (!mons->pack)
			mons->pack = pack_init ();
		for (i = 0; i < pickup->len; ++ i)
		{
			itemID = *(TID*)v_at (pickup, i);
			item = ITEMID(itemID);
			struct Item *packitem;
			for (j = 0; j < MAX_ITEMS_IN_PACK; ++ j)
			{
				packitem = &mons->pack->items[j];
				if (it_can_merge (mons, packitem, item))
					break;
			}
			/* Pick up the item */
			item_put (item, (union ItemLoc) { .inv = {LOC_INV, thID, j}});
			/* Say so */
			char *msg = get_inv_line (packitem);
			p_msg ("%s", msg);
			free (msg);
		}
		if (i < pickup->len)
		{
			p_msg ("No more space. :/");
			break;
		}
		v_free (pickup);
		return;
	case EV_MDROP:
		mons = MTHIID (ev->mdrop.thID);
		if (!mons)
			return;
		items = ev->mdrop.items;
		for (i = 0; i < items->len; ++ i)
		{
			struct Item *drop = ITEMID(*(TID*)v_at (items, i));
			if (item_worn(drop))
				continue;
			item_put (drop, (union ItemLoc) { .dlvl = {LOC_DLVL, mons->dlevel, mons->yloc, mons->xloc}});
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
		if (to->ctr.mode == CTR_AI_TIMID)
			p_msg ("The %s angers the %s!", fr->mname, to->mname);
		to->ctr.mode = CTR_AI_AGGRO;
		to->ctr.aggro.ID = frID;
		return;
	case EV_MCALM:
		thID = ev->mcalm.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		p_msg ("The %s calms.", mons->mname);
		mons->ctr.mode = CTR_AI_TIMID;
		return;
	case EV_MSTARTCHARGE:
		thID = ev->mstartcharge.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		mons->status.charging = 1;
		return;
	case EV_MDOCHARGE:
		thID = ev->mdocharge.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		return;
	case EV_MSTOPCHARGE:
		thID = ev->mstopcharge.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		mons->status.charging = 0;
		return;
	case EV_MFIREBALL:
		thID = ev->mfireball.thID;
		mons = MTHIID (thID);
		if (!mons)
			return;
		newitem = new_item (ityps[ITYP_FIREBALL]);
		loc = (union ItemLoc) { .fl =
			{LOC_FLIGHT, mons->dlevel, mons->yloc, mons->xloc, {0,}, mons->str, thID}};
		bres_init (&loc.fl.bres, mons->yloc, mons->xloc, ev->mfireball.ydest, ev->mfireball.xdest);
		item = item_put (&newitem, loc);
		ev_queue (60, (union Event) { .proj_move = {EV_PROJ_MOVE, item->ID}});
		return;
	case EV_MWATER_BOLT:
		thID = ev->mwater_bolt.thID;
		mons = MTHIID (thID);
		if (!mons)
			return;
		newitem = new_item (ityps[ITYP_WATER_BOLT]);
		loc = (union ItemLoc) { .fl =
			{LOC_FLIGHT, mons->dlevel, mons->yloc, mons->xloc, {0,}, mons->str, thID}};
		bres_init (&loc.fl.bres, mons->yloc, mons->xloc, ev->mwater_bolt.ydest, ev->mwater_bolt.xdest);
		item = item_put (&newitem, loc);
		ev_queue (60, (union Event) { .proj_move = {EV_PROJ_MOVE, item->ID}});
		return;
	case EV_MFROST:
		thID = ev->mfrost.thID;
		mons = MTHIID (thID);
		radius = ev->mfrost.radius;
		for (i = -radius; i < radius; ++ i) for (j = -radius; j < radius; ++ j)
		{
			if (i*i + j*j > radius*radius)
				continue;
			mons_tilefrost (mons, ev->mfrost.ydest + i, ev->mfrost.xdest + j);
		}
		return;
	case EV_CIRCLEOFFLAME:
		thID = ev->circleofflame.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		sk_flames_overlay (mons);
		return;
	case EV_MOPENDOOR:
		thID = ev->mopendoor.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		ev_queue (mons->speed, (union Event) { .mwait = {EV_MWAIT, thID}});
		return;
	case EV_MCLOSEDOOR:
		thID = ev->mclosedoor.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		ev_queue (mons->speed, (union Event) { .mwait = {EV_MWAIT, thID}});
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

