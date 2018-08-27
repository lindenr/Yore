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
#include "include/heap.h"

struct Heap *events;

Tick ev_delay (union Event *event)
{
	switch (event->type)
	{
	case EV_NONE:
		break;
	case EV_MTHROW:
		return (MTHIID(event->mdrop.thID))->speed /2;
	case EV_MWAIT:
		return (MTHIID(event->mwait.thID))->speed;
	case EV_MDOMOVE:
		return (MTHIID(event->mdomove.thID))->speed;
	case EV_MDOATTKM:
		return (MTHIID(event->mdoattkm.thID))->speed;
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
	default:
		return 0;
	}
	panic ("ev_delay reached end");
	return 0;
}

void ev_print (struct QEv *qe);
Tick curtick = 0;
long long unsigned curQID = 0;
void ev_queue (Tick udelay, union Event ev)
{
	Tick when = curtick + udelay;
	++ curQID;
	struct QEv qe = {when, curQID, ev};
	h_push (events, &qe);
}

void ev_debug ()
{
	/*int i;
	printf ("curtick is %llu\n", curtick);
	for (i = 0; i < events->len; ++ i)
	{
		struct QEv *qe = v_at (events, i);
		printf ("due at %llu: %d\n", qe->tick, qe->ev.type);
		if (qe->ev.type == EV_MPOLL)
			printf ("     id %d\n", qe->ev.mpoll.thID);
	}
	printf("\n");*/
}

void ev_print (struct QEv *qe)
{
	//printf ("list %d %d\n", qe->tick, qe->ev.type);
}

void ev_mons_start (struct Monster *mons)
{
	ev_queue (1000 + rn(1000), (union Event) { .mpoll = {EV_MPOLL, mons->ID}});
	ev_queue (1, (union Event) { .mregen = {EV_MREGEN, mons->ID}});
}

int ev_should_refresh = 0;

void ev_do (const union Event *ev)
{
	struct Monster *mons, *fr, *to;
	struct Item newitem, *item;
	TID thID, frID, toID, itemID, monsID;
	struct DLevel *dlvl;
	int ydest, xdest, damage, radius, arm, delay;
	int i, j, ydist, xdist, dist;
	Vector pickup, items;
	union ItemLoc loc;
	switch (ev->type)
	{
	case EV_WORLD_INIT:
		ev_queue (0, (union Event) { .player_init = {EV_PLAYER_INIT}});
		ev_queue (0, (union Event) { .world_heartbeat = {EV_WORLD_HEARTBEAT}});
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

		for (i = 0; i < 100; ++ i)
		{
			mons = gen_mons_in_level ();
			if (!mons)
				continue;
			ev_mons_start (mons);
		}
		//draw_map ();
		return;
	case EV_WORLD_HEARTBEAT:
		/* monster generation */
		//if (onein (3))
		//	ev_queue (0, (union Event) { .mgen = {EV_MGEN}});
		/* next heartbeat */
		ev_queue (1000, (union Event) { .world_heartbeat = {EV_WORLD_HEARTBEAT}});
		return;
	case EV_MTHROW:
		thID = ev->mthrow.thID;
		mons = MTHIID (thID);
		if (!mons)
			return;
		itemID = ev->mthrow.itemID;
		item = it_at (itemID);
		if (!item)
			return mons_poll (mons);
		int speed = mons_throwspeed (mons, item);
		if (speed <= 0)
		{
			eff_mons_fail_throw (mons, item);
			return mons_poll (mons);
		}
		loc = (union ItemLoc) { .fl =
			{LOC_FLIGHT, mons->dlevel, mons->yloc, mons->xloc, {0,}, speed, thID}};
		bres_init (&loc.fl.bres, mons->yloc, mons->xloc, ev->mthrow.ydest, ev->mthrow.xdest);
		item_put (item, loc);
		ev_queue (60, (union Event) { .proj_move = {EV_PROJ_MOVE, itemID}});
		return mons_poll (mons);
	case EV_PROJ_MOVE:
		itemID = ev->proj_move.itemID;
		item = it_at(itemID);
		if (!it_flight (item))
			return;
		if (item->loc.fl.speed <= 0)
		{
			ev_queue (0, (union Event) { .proj_done = {EV_PROJ_DONE, itemID}});
			return;
		}
		memcpy (&loc, &item->loc, sizeof(loc));
		bres_iter (&loc.fl.bres);
		if (!map_passable (dlv_lvl(loc.fl.dlevel), loc.fl.bres.cy, loc.fl.bres.cx))
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
		ev_should_refresh = 1;
		return;
	case EV_PROJ_DONE:
		itemID = ev->proj_done.itemID;
		item = it_at(itemID);
		if (!item)
			return;
		if (it_fragile (item))
		{
			// eff_item_breaks (item); TODO
			it_break (item);
			return;
		}
		if (!it_persistent (item))
		{
			eff_item_dissipates (item);
			item_free (item);
			return;
		}
		item_put (item, (union ItemLoc) { .dlvl =
			{LOC_DLVL, item->loc.fl.dlevel, item->loc.fl.yloc, item->loc.fl.xloc}});
		return;
	case EV_PROJ_HIT_BARRIER:
		itemID = ev->proj_hit_barrier.itemID;
		item = it_at(itemID);
		if (!it_persistent (item))
		{
			eff_item_absorbed (item);
			item_free (item);
			return;
		}
		eff_item_hits_wall (item);
		item_put (item, (union ItemLoc) { .dlvl =
			{LOC_DLVL, item->loc.fl.dlevel, item->loc.fl.yloc, item->loc.fl.xloc}});
		return;
	case EV_PROJ_HIT_MONSTER:
		itemID = ev->proj_hit_monster.itemID;
		item = it_at(itemID);
		if (!it_flight(item))
			return;
		monsID = ev->proj_hit_monster.monsID;
		mons = MTHIID(monsID);
		if (!mons)
			return;
		mons_anger (MTHIID (item->loc.fl.frID), mons);
		if (!proj_hitm (item, mons))
		{
			eff_proj_misses_mons (item, mons);
			return;
		}
		damage = proj_hitdmg (item, mons);
		eff_proj_hits_mons (item, mons, damage);
		fr = MTHIID (item->loc.fl.frID);
		mons_take_damage (mons, fr, damage, it_dtyp (item));
		item->loc.fl.speed = 0;
		if (!fr)
			return;
		if (mons_gets_exp (fr))
			mons_exercise (fr, item);
		return;
	case EV_ITEM_EXPLODE:
		itemID = ev->item_explode.itemID;
		item = it_at(itemID);
		if (it_no (item))
			return;
		ydest = item->loc.fl.yloc; xdest = item->loc.fl.xloc;
		struct BresState bres;
		int R = ev->item_explode.force;
		int r2 = (R-1)*(R-1), R2 = (R+1)*(R+1);
		for (i = -R; i <= R; ++ i)
			for (j = -R; j <= R; ++ j)
			{
				int d2 = i*i + j*j;
				if (d2 >= r2 && d2 <= R2)
				{
					bres_init (&bres, ydest, xdest, ydest + i, xdest + j);
					ev_queue (0, (union Event) { .line_explode = {EV_LINE_EXPLODE, item->loc.fl.dlevel, bres, 0}});
				}
			}
		item_free (item);
		return;
	case EV_LINE_EXPLODE:
		bres = ev->line_explode.bres;
		dlvl = dlv_lvl (ev->line_explode.dlevel);
		ydist = bres.cy - bres.fy; xdist = bres.cx - bres.fx; dist = ev->line_explode.dist;
		if (ydist * ydist + xdist * xdist >= dist * dist)
		{
			ev_queue (50, (union Event) { .line_explode = {EV_LINE_EXPLODE, ev->line_explode.dlevel, bres, ev->line_explode.dist + 1}});
			return;
		}
		dlv_tile_burn (dlvl, bres.cy, bres.cx);
		i = map_buffer (bres.cy, bres.cx);
		if (dlvl->num_fires[i])
		{
			dlvl->num_fires[i] --;
			draw_map_buf (dlvl, i);
		}
		if (bres.done)
			return;
		bres_iter (&bres);
		if (!map_passable (dlvl, bres.cy, bres.cx))
			return;
		i = map_buffer (bres.cy, bres.cx);
		dlvl->num_fires[i] ++;
		draw_map_buf (dlvl, i);
		ev_queue (50, (union Event) { .line_explode = {EV_LINE_EXPLODE, ev->line_explode.dlevel, bres, ev->line_explode.dist + 1}});
		ev_should_refresh = 1;
		return;
	case EV_MWAIT:
		thID = ev->mwait.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		return mons_poll (mons);
	case EV_MMOVE:
		thID = ev->mmove.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		delay = mons->status.flashing.end ? mons->status.flashing.speed : mons->speed;
		ev_queue (delay, (union Event) { .mdomove = {EV_MDOMOVE, thID}});
		mons_start_move (mons, ev->mmove.ydir, ev->mmove.xdir, curtick + delay);
		return;
	case EV_MDOMOVE:
		thID = ev->mdomove.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		ydest = mons->yloc + mons->status.moving.ydir; xdest = mons->xloc + mons->status.moving.xdir;
		if (mons_can_move (mons, mons->status.moving.ydir, mons->status.moving.xdir))
			mons_move (mons, mons->dlevel, ydest, xdest);
		//else: tried and failed to move TODO
		mons_stop_move (mons);
		if (mons->mflags & FL_SLIMY && onein (3))
		{
			// TODO check if there is already slime
			new_thing (THING_DGN, dlv_lvl (mons->dlevel), mons->yloc, mons->xloc, &map_items[DGN_SLIME]);
		}
		return mons_poll (mons);
	case EV_MEVADE:
		thID = ev->mevade.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		delay = mons->speed/3;
		ev_queue (delay, (union Event) { .mdoevade = {EV_MDOEVADE, thID}});
		mons_start_evade (mons, ev->mevade.ydir, ev->mevade.xdir, curtick + delay, curtick + delay + mons->speed);
		return mons_poll (mons);
	case EV_MDOEVADE:
		thID = ev->mdoevade.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		ydest = mons->yloc + mons->status.evading.ydir; xdest = mons->xloc + mons->status.evading.xdir;
		if (mons_can_move (mons, mons->status.evading.ydir, mons->status.evading.xdir))
			mons_move (mons, mons->dlevel, ydest, xdest);
		ev_queue (mons->speed, (union Event) { .munevade = {EV_MUNEVADE, mons->ID}});
		return mons_poll (mons);
	case EV_MUNEVADE:
		thID = ev->munevade.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		mons_stop_evade (mons);
		return mons_poll (mons);
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
		return mons_poll (mons);
	case EV_MATTKM:
		thID = ev->mattkm.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		delay = mons->speed;
		ev_queue (delay, (union Event) { .mdoattkm = {EV_MDOATTKM, thID}});
		mons_start_hit (mons, ev->mattkm.ydir, ev->mattkm.xdir, ev->mattkm.arm, curtick + delay);
		return;
	case EV_MDOATTKM:
		frID = ev->mdoattkm.thID;
		fr = MTHIID(frID); /* get from-mons */
		if (!fr)
			return;
		ydest = fr->yloc + fr->status.attacking.ydir; xdest = fr->xloc + fr->status.attacking.xdir;
		arm = fr->status.attacking.arm;
		mons_stop_hit (fr);
		toID = dlv_lvl(fr->dlevel)->monsIDs[map_buffer(ydest, xdest)]; /* get to-mons */
		if (!toID)
			return mons_poll (fr); // tried and failed to attack TODO
		to = MTHIID(toID);
		mons_anger (fr, to);
		struct Item *with = fr->wearing.weaps[arm];
		int stamina_cost = mons_ST_hit (fr, with);
		if (fr->ST < stamina_cost)
		{
			eff_mons_tiredly_misses_mons (fr, to);
			return mons_poll (fr);
		}
		fr->ST -= stamina_cost;
		if (!mons_hitm (fr, to, with))
		{
			eff_mons_misses_mons (fr, to);
			return mons_poll (fr);
		}
		damage = mons_hitdmg (fr, to, with);
		if (damage == 0)
		{
			eff_mons_just_misses_mons (fr, to);
			return mons_poll (fr);
		}
		eff_mons_hits_mons (fr, to, damage);
		if (mons_gets_exp (fr))
			mons_exercise (fr, with);
		if (!mons_take_damage (to, fr, damage, it_dtyp (with)))
			return mons_poll (fr);
		if ((!it_no(with)) && (it_ityp (with)->flags & ITF_FIRE_EFFECT))
		{
			damage = rn(5);
			eff_mons_burns (to, damage);
			if (!mons_take_damage (to, fr, damage, DTYP_FIRE))
				return mons_poll (fr);
		}
		return mons_poll (fr);
	case EV_MPOLL:
		mons = MTHIID(ev->mpoll.thID);
		if (!mons)
			return;
		return mons_poll (mons);
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
	case EV_MBLEED:
		mons = MTHIID(ev->mbleed.thID);
		if (!mons)
			return;
		if (!mons->status.bleeding)
			return;
		damage = rn(5);
		eff_mons_bleeds (mons, damage);
		if (!mons_take_damage (mons, NULL, damage, DTYP_BLEED))
			return;
		ev_queue (1000, (union Event) { .mbleed = {EV_MBLEED, mons->ID}});
		return;
	case EV_MWIELD:
		mons = MTHIID(ev->mwield.thID);
		if (!mons)
			return;
		arm = ev->mwield.arm;
		if (mons->status.attacking.arm == arm)
			return mons_poll (mons);
		if (mons->wearing.weaps[arm])
			mons_unwield (mons, mons->wearing.weaps[arm]);
		struct Item *it = it_at(ev->mwield.itemID);
		if (it_invID (it) != ev->mwield.thID)
		{
			eff_mons_unwields (mons);
			return mons_poll (mons);
		}
		eff_mons_wields_item (mons, it);
		mons_wield (mons, arm, it);
		return mons_poll (mons);
	case EV_MWEAR_ARMOUR:
		mons = MTHIID(ev->mwear_armour.thID);
		if (!mons)
			return;
		item = it_at (ev->mwear_armour.itemID); 
		if (it_invID (item) != ev->mwear_armour.thID ||
			it_worn (item))
			return mons_poll (mons);
		if (!mons_can_wear (mons, item, ev->mwear_armour.offset))
			return mons_poll (mons);
		eff_mons_wears_item (mons, item);
		mons_wear (mons, item, ev->mwear_armour.offset);
		return mons_poll (mons);
	case EV_MTAKEOFF_ARMOUR:
		mons = MTHIID(ev->mtakeoff_armour.thID);
		if (!mons)
			return;
		item = it_at (ev->mtakeoff_armour.itemID); 
		if (it_invID(item) != ev->mtakeoff_armour.thID ||
			(!it_worn(item)))
			return mons_poll (mons);
		if (!mons_can_takeoff (mons, item))
			return mons_poll (mons);
		mons_take_off (mons, item);
		eff_mons_takes_off_item (mons, item);
		return mons_poll (mons);
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
			item = it_at(itemID);
			struct Item *packitem;
			//for (j = 0; j < MAX_ITEMS_IN_PACK; ++ j)
			//{
			//	packitem = &mons->pack->items[j];
				//if (it_can_merge (packitem, item) && !it_no(packitem))
				//	break;
			//}
			//if (j < MAX_ITEMS_IN_PACK)
			//	goto pick_up_item;
			for (j = 0; j < MAX_ITEMS_IN_PACK; ++ j)
			{
				packitem = &mons->pack->items[j];
				if (it_no (packitem))//it_can_merge (packitem, item))
					break;
			}
		//pick_up_item:
			/* Pick up the item */
			item_put (item, (union ItemLoc) { .inv = {LOC_INV, thID, j}});
			/* Say so */
			eff_mons_picks_up_item (mons, packitem);
		}
		if (i < pickup->len)
		{
			p_msg ("No more space. :/");
			break;
		}
		v_free (pickup);
		return mons_poll (mons);
	case EV_MDROP:
		mons = MTHIID (ev->mdrop.thID);
		if (!mons)
			return;
		items = ev->mdrop.items;
		for (i = 0; i < items->len; ++ i)
		{
			struct Item *drop = it_at(*(TID*)v_at (items, i));
			if (it_worn(drop))
				continue;
			item_put (drop, (union ItemLoc) { .dlvl = {LOC_DLVL, mons->dlevel, mons->yloc, mons->xloc}});
		}
		v_free (items);
		return mons_poll (mons);
	case EV_MSTARTCHARGE:
		thID = ev->mstartcharge.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		mons->status.charging = 1;
		return mons_poll (mons);
	case EV_MDOCHARGE:
		thID = ev->mdocharge.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		return mons_poll (mons);
	case EV_MSTOPCHARGE:
		thID = ev->mstopcharge.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		mons->status.charging = 0;
		return mons_poll (mons);
	case EV_MFIREBALL:
		thID = ev->mfireball.thID;
		mons = MTHIID (thID);
		if (!mons)
			return;
		newitem = new_item (ITYP_FIREBALL);
		it_set_attk (&newitem, ev->mfireball.attk);
		loc = (union ItemLoc) { .fl =
			{LOC_FLIGHT, mons->dlevel, mons->yloc, mons->xloc, {0,}, mons->str, thID}};
		bres_init (&loc.fl.bres, mons->yloc, mons->xloc, ev->mfireball.ydest, ev->mfireball.xdest);
		item = item_put (&newitem, loc);
		ev_queue (60, (union Event) { .proj_move = {EV_PROJ_MOVE, item->ID}});
		return mons_poll (mons);
	case EV_MWATER_BOLT:
		thID = ev->mwater_bolt.thID;
		mons = MTHIID (thID);
		if (!mons)
			return;
		newitem = new_item (ITYP_WATER_BOLT);
		it_set_attk (&newitem, ev->mwater_bolt.attk);
		loc = (union ItemLoc) { .fl =
			{LOC_FLIGHT, mons->dlevel, mons->yloc, mons->xloc, {0,}, mons->str, thID}};
		bres_init (&loc.fl.bres, mons->yloc, mons->xloc, ev->mwater_bolt.ydest, ev->mwater_bolt.xdest);
		item = item_put (&newitem, loc);
		ev_queue (60, (union Event) { .proj_move = {EV_PROJ_MOVE, item->ID}});
		return mons_poll (mons);
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
		return mons_poll (mons);
	case EV_CIRCLEOFFLAME:
		thID = ev->circleofflame.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		sk_flames_overlay (mons);
		return mons_poll (mons);
	case EV_MFLASH:
		thID = ev->mflash.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		if (mons->status.flashing.end)
			return mons_poll (mons);
		mons->status.flashing.end = curtick + ev->mflash.duration;
		mons->status.flashing.speed = ev->mflash.speed;
		ev_queue (ev->mflash.duration, (union Event) { .mstopflash = {EV_MSTOPFLASH, thID}});
		return mons_poll (mons);
	case EV_MSTOPFLASH:
		thID = ev->mstopflash.thID;
		mons = MTHIID(thID);
		if (!mons)
			return;
		mons->status.flashing.end = 0;
		return mons_poll (mons);
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

int qev_cmp (struct QEv *q1, struct QEv *q2)
{
	if (q1->tick < q2->tick)
		return 1;
	return q1->tick == q2->tick && q1->ID < q2->ID;
}

void ev_init ()
{
	events = h_dinit (sizeof(struct QEv), qev_cmp);
	player_actions = v_dinit (sizeof(struct QEv));
	ev_queue (0, (union Event) { .world_init = {EV_WORLD_INIT}});
}

void ev_loop ()
{
	while (U.playing == PLAYER_PLAYING)
	{
		////printf ("CURTICK=%d\n", curtick);
		const struct QEv *qe = h_least (events);
		if (curtick/50 < qe->tick/50 && ev_should_refresh)
		{
			ev_should_refresh = 0;
			p_pane (NULL);
			gr_refresh ();
			gr_wait (50, 1);
		}
		curtick = qe->tick;

		ev_do (&qe->ev);
		h_pop (events, NULL);
	}
}

