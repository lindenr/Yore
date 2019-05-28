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
#include "include/map.h"

#include <stdio.h>

struct Heap *events;

Tick curtick = 0;
static EvID curQID = 0; /* no event has ID 0 */
struct QEv *ev_queue_aux (Tick udelay, union Event ev)
{
	Tick when = curtick + udelay;
	++ curQID;
	struct QEv qe = {curQID, when, ev};
	return h_push (events, &qe);
}

#include "auto/event.queue.h"

static int ev_should_refresh = 0;

void ev_world_init ()
{
#ifndef SIM
	ev_queue (0, player_init);
	generate_map (dlv_lvl (1), LEVEL_3D);
#else
	generate_map (dlv_lvl (1), LEVEL_SIM);
	update_knowledge (NULL);
	//gra_centcam (map_graph, 50, 150);
#endif /* SIM */
	ev_queue (0, world_heartbeat);
}

void ev_player_init ()
{
	MonsID mons = gen_player (1, 50, 50, player_name);
	dlv_fill_player_dist (cur_dlevel);
	update_knowledge (mons);

#ifdef TWOPLAYER
	gen_player (1, 52, 53, "Player 2");
	dlv_fill_player_dist (cur_dlevel);
	update_knowledge (mons);
#endif
	
	//gen_boss (1, 57, 60);

	int i;
	for (i = 0; i < 60; ++ i)
	{
		//gen_mons_in_level ();
		//gen_mons_near_player ();
	}
	//draw_map ();
}

void ev_world_heartbeat ()
{
	/* monster generation */
	//if (onein (2))
		ev_queue (0, mgen);
	/* next heartbeat */
	ev_queue (1000, world_heartbeat);
#ifdef SIM
	//
#endif /* SIM */
}

void ev_itrot (ItemID item)
{
	it_destroy (item);
}

void ev_mthrow (MonsID mons, ItemID item, int ydest, int xdest)
{
	int speed = mons_throwspeed (mons, item);
	if (speed <= 0)
	{
		eff_mons_fail_throw (mons, item);
		return;
	}
	union ItemLoc loc = it_monsdloc (mons);
	struct BresState bres;
	bres_init (&bres, loc.dlvl.yloc, loc.dlvl.xloc, ydest, xdest);
	it_put (item, loc);
	// TODO: delay (i.e. actual speed) depends on speed variable?
	ev_queue (60, proj_move, item, bres, speed, mons);
}

void proj_done (ItemID item)
{
	if (it_fragile (item))
	{
		// eff_item_breaks (item); TODO
		it_break (item);
		return;
	}
	if (!it_persistent (item))
	{
		eff_item_dissipates (item);
		it_destroy (item);
		return;
	}
}

void proj_hit_barrier (ItemID item)
{
	if (!it_persistent (item))
	{
		eff_item_absorbed (item);
		it_destroy (item);
		return;
	}
	eff_item_hits_wall (item);
}

void proj_hit_monster (ItemID item, MonsID mons, MonsID from)
{
	struct ItemInDlvl dl;
	if (!it_dlvl (item, &dl))
		return;
	//mons_anger (fl.frID, mons); TODO: frID should be
	// included in proj_move event; cannot do at moment
	// because then python would abort the event if the monster dies!
	if (!proj_hitm (item, mons))
	{
		eff_proj_misses_mons (item, mons);
		return;
	}
	int damage = proj_hitdmg (item, mons);
	eff_proj_hits_mons (item, mons, damage);
	mons_take_damage (mons, 0, damage, it_dtyp (item));

	//it_put (item, (union ItemLoc) {.fl = fl});
	if (!from)
		return;
	if (mons_gets_exp (from))
		mons_exercise (from, item);
}

void ev_proj_move (ItemID item, struct BresState bres, int speed, MonsID from)
{
	struct ItemInDlvl dlvl;
	if (!it_dlvl (item, &dlvl))
		return; // ??
	if (speed <= 0)
	{
		proj_done (item);
		return;
	}
	bres_iter (&bres);
	struct DLevel *lvl = dlv_lvl (dlvl.dlevel);
	if (!map_passable (lvl, dlvl.zloc, bres.cy, bres.cx))
	{
		proj_hit_barrier (item);
		return;
	}
	dlvl.yloc = bres.cy;
	dlvl.xloc = bres.cx;
	speed -= 1;
	it_put (item, (union ItemLoc) {.dlvl = dlvl});
	MonsID mons = lvl->monsIDs[dlv_index (lvl, dlvl.zloc, dlvl.yloc, dlvl.xloc)];
	if (mons)
	{
		proj_hit_monster (item, mons, from);
		speed = 0;
	}
	ev_queue (60, proj_move, item, bres, speed, from);
	ev_should_refresh = 1;
}

void ev_item_explode (ItemID item, int force)
{
	struct ItemInDlvl dl;
	if (!it_dlvl(item, &dl))
		return; // ??
	int y = dl.yloc, x = dl.xloc;
	int R = force;
	int r2 = (R-1)*(R-1), R2 = (R+1)*(R+1);
	int i, j;
	struct BresState bres;
	for (i = -R; i <= R; ++ i) for (j = -R; j <= R; ++ j)
	{
		int d2 = i*i + j*j;
		if (d2 >= r2 && d2 <= R2)
		{
			bres_init (&bres, y, x, y + i, x + j);
			ev_queue (0, line_explode, dl.dlevel, dl.zloc, bres, 0);
		}
	}
	it_destroy (item);
}

void ev_line_explode (int dlevel, int zloc, struct BresState bres, int dist)
{
	struct DLevel *lvl = dlv_lvl (dlevel);
	int ydist = bres.cy - bres.fy, xdist = bres.cx - bres.fx;
	if (ydist * ydist + xdist * xdist >= dist * dist)
	{
		ev_queue (50, line_explode, dlevel, zloc, bres, dist + 1);
		return;
	}
	dlv_tile_burn (lvl, zloc, bres.cy, bres.cx);
	int i = dlv_index (lvl, zloc, bres.cy, bres.cx);
	if (lvl->num_fires[i])
	{
		lvl->num_fires[i] --;
		draw_map_buf (lvl, i);
	}
	if (bres.done)
		return;
	bres_iter (&bres);
	if (!map_passable (lvl, zloc, bres.cy, bres.cx))
		return;
	i = dlv_index (lvl, zloc, bres.cy, bres.cx);
	lvl->num_fires[i] ++;
	draw_map_buf (lvl, i);
	ev_queue (50, line_explode, dlevel, zloc, bres, dist + 1);
	ev_should_refresh = 1;
}

void ev_mdomove (MonsID mons, int zdir, int ydir, int xdir)
{
	int d, z, y, x;
	mons_getloc (mons, &d, &z, &y, &x);
	int zdest = z + zdir;
	int ydest = y + ydir;
	int xdest = x + xdir;
	if (mons_can_move (mons, zdir, ydir, xdir))
		mons_move (mons, mons_dlevel (mons), zdest, ydest, xdest);
	// eff_mons_fails_move TODO
	//if (mons->mflags & FL_SLIMY && onein (3))
	{
		// TODO check if there is already slime
		//new_thing (THING_DGN, dlv_lvl (mons->dlevel), mons->zloc, mons->yloc, mons->xloc, &map_items[DGN_SLIME]);
	}
}

void ev_mdoevade (MonsID mons, int zdir, int ydir, int xdir)
{
	int d, z, y, x;
	mons_getloc (mons, &d, &z, &y, &x);
	int zdest = z + zdir;
	int ydest = y + ydir;
	int xdest = x + xdir;
	if (mons_can_move (mons, zdir, ydir, xdir))
		mons_move (mons, mons_dlevel (mons), zdest, ydest, xdest);
	ev_queue (mons_speed (mons), munevade, mons);
}

void ev_munevade (MonsID mons)
{
}

void ev_mshield (MonsID mons, int ydir, int xdir)
{
	ev_queue (mons_speed (mons)/2, mdoshield, mons, ydir, xdir);
}

void ev_mdoshield (MonsID mons, int ydir, int xdir)
{
	ev_queue ((mons_speed (mons)+1)/2, munshield, mons);
}

void ev_munshield (MonsID mons)
{
}

void ev_mdohit (MonsID fr, int arm, int zdir, int ydir, int xdir)
{
	int d, z, y, x;
	mons_getloc (fr, &d, &z, &y, &x);
	int zdest = z + zdir;
	int ydest = y + ydir;
	int xdest = x + xdir;
	MonsID to = dlv_mvmons (mons_dlevel (fr), zdest, ydest, xdest); /* get to-mons */
	if (!to)
	{
		//eff_mons_swings_wildly (fr); TODO
		return;
	}
	mons_anger (fr, to);
	ItemID with = mons_getweap (fr, arm);
	//int stamina_cost = mons_ST_hit (fr, with);
	/*if (!mons_ST_lose (fr, stamina_cost))
	{
		eff_mons_tiredly_misses_mons (fr, to);
		return;
	}TODO put in mons_hitm */
	if (!mons_hitm (fr, to, with))
	{
		// check error message
		eff_mons_misses_mons (fr, to);
		return;
	}
	int damage = mons_hitdmg (fr, to, with);
	if (damage == 0)
	{
		eff_mons_just_misses_mons (fr, to);
		return;
	}
	eff_mons_hits_mons (fr, to, damage);
	if (mons_gets_exp (fr))
		mons_exercise (fr, with);
	if (!mons_take_damage (to, fr, damage, it_dtyp (with)))
		return;
	if (with && (it_flag (with, ITF_FIRE_EFFECT)))
	{
		int i, j;
		for (i = -1; i <= 1; ++ i) for (j = -1; j <= 1; ++ j)
		{
			struct BresState bres;
			bres_init (&bres, ydest, xdest, ydest + 2*ydir + i, xdest + 2*xdir + j);
			ev_queue (10 + rn(40), line_explode, mons_dlevel(fr), z, bres, 0);
		}
	}
}

void ev_mpoll (MonsID mons)
{
	//mons_poll (mons);
}

void ev_mgen ()
{
	//mons = gen_mons_near_player ();
}

void ev_mregen (MonsID mons)
{
	ev_queue (mons_tregen (mons), mregen, mons); /* next regen */
	// TODO all goes in monst.h
	/* HP */
	//int HP_regen = mons_HP_regen (mons);
	//mons->HP += HP_regen;
	//if (mons->HP > mons->HP_max)
	//	mons->HP = mons->HP_max;
	/* ST */
	//int ST_regen = mons_ST_regen (mons);
	//mons->ST += ST_regen;
	//if (mons->ST > mons->ST_max)
	//	mons->ST = mons->ST_max;
	/* MP */
	//int MP_regen = mons_MP_regen (mons);
	//mons->MP += MP_regen;
	//if (mons->MP > mons->MP_max)
	//	mons->MP = mons->MP_max;
}

void ev_mbleed (MonsID mons)
{
	int damage = rn(5);
	eff_mons_bleeds (mons, damage);
	if (!mons_take_damage (mons, 0, damage, DTYP_BLEED))
		return;
	ev_queue (1000, mbleed, mons);
}

void ev_mwield (MonsID mons, int arm, ItemID item)
{
	if (mons_getweap (mons, arm))
		mons_unwield (mons, arm);
	ev_queue ((mons_speed (mons)+1)/2, mdowield, mons, arm, item);
}

void ev_mdowield (MonsID mons, int arm, ItemID item)
{
	//if (it_inv (item) != mons) // should be captured
	//{
	//	eff_mons_unwields (mons);
	//	return;
	//}
	eff_mons_wields_item (mons, item);
	mons_wield (mons, arm, item);
}

void ev_mwear_armour (MonsID mons, ItemID item, size_t offset)
{
	/*if (!mons_can_wear (mons, item, offset))
	{
		// eff_mons_fails_wear TODO check error
		return;
	}*/
	eff_mons_wears_item (mons, item);
	mons_wear (mons, item, offset);
}

void ev_mtakeoff_armour (MonsID mons, ItemID item)
{
	/*if (!mons_can_takeoff (mons, item))
		return; // TODO why not?*/
	mons_take_off (mons, item);
	eff_mons_takes_off_item (mons, item);
}

void ev_mpickup (MonsID mons, V_ItemID pickup)
{
	/* Put items in ret_list into inventory. The loop
	 * continues until ret_list is done or the pack is full. */
	struct Pack *pack = mons_pack (mons);
	int i, j;
	for (i = 0; i < pickup->len; ++ i)
	{
		ItemID item = pickup->data[i], ID;
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
			ID = pack->items[j];
			if (!ID)//it_can_merge (packitem, item))
				break;
		}
		if (j >= MAX_ITEMS_IN_PACK)
			break;
	//pick_up_item:
		/* Pick up the item */
		it_put (item, (union ItemLoc) { .inv = {LOC_INV, mons, j}});
		/* Say so */
		eff_mons_picks_up_item (mons, item);
	}
	if (i < pickup->len)
		p_msg ("No more space. :/");
	v_free (pickup);
}

void ev_mdrop (MonsID mons, V_ItemID items)
{
	int d, z, y, x;
	mons_getloc (mons, &d, &z, &y, &x);
	int i;
	union ItemLoc loc = it_monsdloc (mons);
	for (i = 0; i < items->len; ++ i)
	{
		ItemID drop = items->data[i];
		if (it_worn (drop))
			continue;
		it_put (drop, loc);
	}
	v_free (items);
}

void ev_mstartcharge (MonsID mons)
{
}

void ev_mdocharge (MonsID mons)
{
}

void ev_mstopcharge (MonsID mons)
{
}

void ev_mfireball (MonsID mons, int ydest, int xdest, int attk)
{
	int d, z, y, x;
	mons_getloc (mons, &d, &z, &y, &x);
	struct Item_internal newitem = new_item (ITYP_FIREBALL);

	union ItemLoc loc = it_monsdloc (mons); //, 10); //mons->str); // TODO
	struct BresState bres;
	bres_init (&bres, y, x, ydest, xdest);
	
	ItemID item = it_create (&newitem, loc);
	it_set_attk (item, attk);
	ev_queue (60, proj_move, item, bres, 10, mons);
}

void ev_mwater_bolt (MonsID mons, int ydest, int xdest, int attk)
{
	int d, z, y, x;
	mons_getloc (mons, &d, &z, &y, &x);
	struct Item_internal newitem = new_item (ITYP_WATER_BOLT);

	union ItemLoc loc = it_monsdloc (mons); //, 10); //mons->str); // TODO
	struct BresState bres;
	bres_init (&bres, y, x, ydest, xdest);
	
	ItemID item = it_create (&newitem, loc);
	it_set_attk (item, attk);
	ev_queue (60, proj_move, item, bres, 10, mons);
}

void ev_mfrost (MonsID mons, int zdest, int ydest, int xdest, int radius)
{
	int i, j;
	for (i = -radius; i < radius; ++ i) for (j = -radius; j < radius; ++ j)
	{
		if (i*i + j*j > radius*radius)
			continue;
		mons_tilefrost (mons, zdest, ydest + i, xdest + j);
	}
}

void ev_mflash (MonsID mons, int speed, Tick duration)
{
	ev_queue (duration, mstopflash, mons);
}

void ev_mstopflash (MonsID mons)
{
}

void ev_mopendoor (MonsID mons, int ydest, int xdest)
{
	ev_queue (mons_speed (mons), mpoll, mons);
}

void ev_mclosedoor (MonsID mons, int ydest, int xdest)
{
	ev_queue (mons_speed (mons), mpoll, mons);
}

void ev_compute (ItemID item)
{
	struct ItemInDlvl dl;
	if (!it_dlvl (item, &dl))
		return;
	MonsID mons = dlv_lvl(dl.dlevel)->playerIDs->data[0];
	int d, z, y, x;
	mons_getloc (mons, &d, &z, &y, &x);
	if (z == dl.zloc && abs(y - dl.yloc) < 10 && abs(x - dl.xloc) < 10)
		it_shoot (item, z, y, x);
	ev_queue (600, compute, item);
}

#include "auto/event.switch.h"

int qev_lt (struct QEv *q1, struct QEv *q2)
{
	if (q1->tick < q2->tick)
		return 1;
	return q1->tick == q2->tick && q1->ID < q2->ID;
}

void ev_init ()
{
	events = h_dinit (sizeof(struct QEv), qev_lt);
	ev_queue (0, world_init);
}

#ifndef SIM
void ev_loop ()
{
	ev_init ();
	while (U.playing == PLAYER_PLAYING)
	{
		////printf ("CURTICK=%d\n", curtick);
		const struct QEv *qe = h_least (events);
		if (curtick/50 < qe->tick/50 && ev_should_refresh)
		{
			ev_should_refresh = 0;
			p_pane (0);
			gr_refresh ();
			//gr_wait (20, 1);
		}
		curtick = qe->tick;

		ev_do (qe);
		h_pop (events, NULL);
	}
}
#else
#include "include/sim.h"
void ev_loop ()
{
	ev_init ();
	while (U.playing == PLAYER_PLAYING)
	{
		////printf ("CURTICK=%d\n", curtick);
		const struct QEv *qe = h_least (events);
		if (curtick/618 < qe->tick/618)
		{
			p_pane (NULL);
			gr_refresh ();
			//gr_wait (80, 0);
			//if (gr_wait (50, 1) != GRK_EOF)
			{
				char in = gr_getch_int (-1);
				while (in != GRK_EOF)
				{
					if (in == GRK_ESC)
						return;
					else
						sim_keypress (in);
					in = gr_getch_int (-1);
				}
			}
		}
		curtick = qe->tick;

		ev_do (qe);
		h_pop (events, NULL);
	}
}
#endif /* SIM */

