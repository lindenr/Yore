// Generated code - don't change directly or it will be overwritten.

#ifndef event_queue_h
#define event_queue_h

void ev_queue_dlevel_heartbeat (Tick udelay, int dlevel)
{
	ev_queue_aux (udelay, (union Event) { .dlevel_heartbeat = {EV_dlevel_heartbeat, dlevel}});
}

void ev_queue_itrot (Tick udelay, ItemID itemID)
{
	ev_queue_aux (udelay, (union Event) { .itrot = {EV_itrot, itemID}});
}

void ev_queue_mthrow (Tick udelay, MonsID monsID, ItemID itemID, int ydest, int xdest)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mthrow = {EV_mthrow, monsID, itemID, ydest, xdest}});
	mons_internal (monsID)->status.throw = (typeof(mons_internal (monsID)->status.throw)) {qev->ID, qev->tick, itemID, ydest, xdest};
}

void ev_queue_proj_move (Tick udelay, ItemID itemID, struct BresState bres, int speed, MonsID frID)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .proj_move = {EV_proj_move, itemID, bres, speed, frID}});
	it_internal (itemID)->status.flight = (typeof(it_internal (itemID)->status.flight)) {qev->ID, qev->tick, bres, speed, frID};
}

void ev_queue_item_explode (Tick udelay, ItemID itemID, int force)
{
	ev_queue_aux (udelay, (union Event) { .item_explode = {EV_item_explode, itemID, force}});
}

void ev_queue_line_explode (Tick udelay, int dlevel, int zloc, struct BresState bres, int dist)
{
	ev_queue_aux (udelay, (union Event) { .line_explode = {EV_line_explode, dlevel, zloc, bres, dist}});
}

void ev_queue_mdomove (Tick udelay, MonsID monsID, int zdir, int ydir, int xdir)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mdomove = {EV_mdomove, monsID, zdir, ydir, xdir}});
	mons_internal (monsID)->status.move = (typeof(mons_internal (monsID)->status.move)) {qev->ID, qev->tick, zdir, ydir, xdir};
}

void ev_queue_mdoevade (Tick udelay, MonsID monsID, int zdir, int ydir, int xdir)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mdoevade = {EV_mdoevade, monsID, zdir, ydir, xdir}});
	mons_internal (monsID)->status.evade = (typeof(mons_internal (monsID)->status.evade)) {qev->ID, qev->tick, zdir, ydir, xdir};
}

void ev_queue_munevade (Tick udelay, MonsID monsID)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .munevade = {EV_munevade, monsID}});
	mons_internal (monsID)->status.evade.evID = qev->ID;
}

void ev_queue_mdoshield (Tick udelay, MonsID monsID, int ydir, int xdir)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mdoshield = {EV_mdoshield, monsID, ydir, xdir}});
	mons_internal (monsID)->status.shield = (typeof(mons_internal (monsID)->status.shield)) {qev->ID, qev->tick, ydir, xdir};
}

void ev_queue_munshield (Tick udelay, MonsID monsID)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .munshield = {EV_munshield, monsID}});
	mons_internal (monsID)->status.shield.evID = qev->ID;
}

void ev_queue_mdohit (Tick udelay, MonsID frID, int arm, int zdir, int ydir, int xdir)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mdohit = {EV_mdohit, frID, arm, zdir, ydir, xdir}});
	mons_internal (frID)->status.hit = (typeof(mons_internal (frID)->status.hit)) {qev->ID, qev->tick, arm, zdir, ydir, xdir};
}

void ev_queue_mpoll (Tick udelay, MonsID monsID)
{
	ev_queue_aux (udelay, (union Event) { .mpoll = {EV_mpoll, monsID}});
}

void ev_queue_mgen (Tick udelay, int dlevel)
{
	ev_queue_aux (udelay, (union Event) { .mgen = {EV_mgen, dlevel}});
}

void ev_queue_mregen (Tick udelay, MonsID monsID)
{
	ev_queue_aux (udelay, (union Event) { .mregen = {EV_mregen, monsID}});
}

void ev_queue_mbleed (Tick udelay, MonsID monsID)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mbleed = {EV_mbleed, monsID}});
	mons_internal (monsID)->status.bleed = (typeof(mons_internal (monsID)->status.bleed)) {qev->ID, qev->tick};
}

void ev_queue_mwield (Tick udelay, MonsID monsID, int arm, ItemID itemID)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mwield = {EV_mwield, monsID, arm, itemID}});
	mons_internal (monsID)->status.wield = (typeof(mons_internal (monsID)->status.wield)) {qev->ID, qev->tick, arm, itemID};
}

void ev_queue_mdowield (Tick udelay, MonsID monsID, int arm, ItemID itemID)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mdowield = {EV_mdowield, monsID, arm, itemID}});
	mons_internal (monsID)->status.wield.evID = qev->ID;
}

void ev_queue_mwear_armour (Tick udelay, MonsID monsID, ItemID itemID, size_t offset)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mwear_armour = {EV_mwear_armour, monsID, itemID, offset}});
	mons_internal (monsID)->status.wear = (typeof(mons_internal (monsID)->status.wear)) {qev->ID, qev->tick, itemID, offset};
}

void ev_queue_mtakeoff_armour (Tick udelay, MonsID monsID, ItemID itemID)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mtakeoff_armour = {EV_mtakeoff_armour, monsID, itemID}});
	mons_internal (monsID)->status.takeoff = (typeof(mons_internal (monsID)->status.takeoff)) {qev->ID, qev->tick, itemID};
}

void ev_queue_mpickup (Tick udelay, MonsID monsID, V_ItemID pickup)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mpickup = {EV_mpickup, monsID, pickup}});
	mons_internal (monsID)->status.pickup = (typeof(mons_internal (monsID)->status.pickup)) {qev->ID, qev->tick, pickup};
}

void ev_queue_mdrop (Tick udelay, MonsID monsID, V_ItemID items)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mdrop = {EV_mdrop, monsID, items}});
	mons_internal (monsID)->status.drop = (typeof(mons_internal (monsID)->status.drop)) {qev->ID, qev->tick, items};
}

void ev_queue_mdocharge (Tick udelay, MonsID monsID)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mdocharge = {EV_mdocharge, monsID}});
	mons_internal (monsID)->status.charge = (typeof(mons_internal (monsID)->status.charge)) {qev->ID, qev->tick};
}

void ev_queue_mstopcharge (Tick udelay, MonsID monsID)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mstopcharge = {EV_mstopcharge, monsID}});
	mons_internal (monsID)->status.charge.evID = qev->ID;
}

void ev_queue_mfireball (Tick udelay, MonsID monsID, int ydest, int xdest, int attk)
{
	ev_queue_aux (udelay, (union Event) { .mfireball = {EV_mfireball, monsID, ydest, xdest, attk}});
}

void ev_queue_mwater_bolt (Tick udelay, MonsID monsID, int ydest, int xdest, int attk)
{
	ev_queue_aux (udelay, (union Event) { .mwater_bolt = {EV_mwater_bolt, monsID, ydest, xdest, attk}});
}

void ev_queue_mfrost (Tick udelay, MonsID monsID, int zdest, int ydest, int xdest, int radius)
{
	ev_queue_aux (udelay, (union Event) { .mfrost = {EV_mfrost, monsID, zdest, ydest, xdest, radius}});
}

void ev_queue_mflash (Tick udelay, MonsID monsID, int speed, Tick duration)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .mflash = {EV_mflash, monsID, speed, duration}});
	mons_internal (monsID)->status.flash = (typeof(mons_internal (monsID)->status.flash)) {qev->ID, qev->tick, speed, duration};
}

void ev_queue_mstopflash (Tick udelay, MonsID monsID)
{
	ev_queue_aux (udelay, (union Event) { .mstopflash = {EV_mstopflash, monsID}});
}

void ev_queue_mopendoor (Tick udelay, MonsID monsID, int ydest, int xdest)
{
	ev_queue_aux (udelay, (union Event) { .mopendoor = {EV_mopendoor, monsID, ydest, xdest}});
}

void ev_queue_mclosedoor (Tick udelay, MonsID monsID, int ydest, int xdest)
{
	ev_queue_aux (udelay, (union Event) { .mclosedoor = {EV_mclosedoor, monsID, ydest, xdest}});
}

void ev_queue_compute (Tick udelay, ItemID itemID)
{
	struct QEv *qev = ev_queue_aux (udelay, (union Event) { .compute = {EV_compute, itemID}});
	it_internal (itemID)->status.compute = (typeof(it_internal (itemID)->status.compute)) {qev->ID, qev->tick};
}


#endif /* event_queue_h */

